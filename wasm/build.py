#!/usr/bin/env python3
# build.py - 使用 PATH 中的 Clang，仅需 QT_DIR 环境变量

import os
import sys
import subprocess
import argparse
from pathlib import Path

# ------------------ 配置 ------------------
PROJECT_ROOT = Path(__file__).parent.resolve()
BUILD_DIR = PROJECT_ROOT / "build"

# 只需要 QT_DIR 环境变量
QT_DIR = os.environ.get("QT6_DIR")

if not QT_DIR:
    print("错误：环境变量 QT_DIR 未设置！")
    print("请设置：setx QT_DIR \"D:/Qt/6.10.0/llvm-mingw_64\"")
    sys.exit(1)

QT_DIR = Path(QT_DIR).resolve()

if not (QT_DIR / "lib/cmake/Qt6/Qt6Config.cmake").exists():
    print(f"错误：在 {QT_DIR} 未找到 Qt6Config.cmake，Qt 路径可能错误")
    sys.exit(1)

# ------------------ 工具检查 ------------------
def check_tool(name):
    result = subprocess.run(["where" if os.name == "nt" else "which", name], 
                          capture_output=True)
    if result.returncode != 0:
        print(f"错误：未找到 {name}，请确保已添加到 PATH")
        sys.exit(1)
    return result.stdout.decode().strip().splitlines()[0]

# 验证 LLVM 工具在 PATH 中
CLANG_PATH = check_tool("clang++")
LLD_LINK_PATH = check_tool("ld.lld")
print(f"使用编译器: {CLANG_PATH}")
print(f"使用链接器: {LLD_LINK_PATH}")

# ------------------ 构建逻辑 ------------------
def run_command(cmd, cwd=None, shell=False):
    print(f"执行: {' '.join(cmd)}")
    result = subprocess.run(
        cmd,
        cwd=cwd,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        bufsize=1,
        shell=shell
    )
    print(result.stdout)
    if result.returncode != 0:
        print(f"命令失败，退出码: {result.returncode}")
        sys.exit(result.returncode)

def ensure_build_dir():
    BUILD_DIR.mkdir(exist_ok=True)
    print(f"构建目录: {BUILD_DIR}")

def is_cache_valid():
    return (BUILD_DIR / "CMakeCache.txt").exists()

def configure(build_type):
    print(f"配置项目 ({build_type})...")
    cmd = [
        "cmake",
        "-S", str(PROJECT_ROOT),
        "-B", str(BUILD_DIR),
        "-G", "Ninja",
        f"-DCMAKE_PREFIX_PATH={QT_DIR}",
        f"-DCMAKE_BUILD_TYPE={build_type}"
        # 不指定 CMAKE_CXX_COMPILER，让 CMake 自动从 PATH 找 clang++
    ]
    run_command(cmd)

def build(build_type):
    print(f"构建项目 ({build_type})...")
    cmd = [
        "cmake",
        "--build", str(BUILD_DIR),
        "--config", build_type,
        "--target", "all",
        "--parallel"
    ]
    run_command(cmd)

def deploy():
    exe_path = BUILD_DIR / "VCOffice.exe"
    if not exe_path.exists():
        print("VCOffice.exe 不存在")
        return

    # 获取可执行文件所在目录（即 build/）
    target_dir = exe_path.parent

    print(f"部署到目录: {target_dir}")

    # --- 1. 使用 windeployqt 部署 Qt 依赖 ---
    print("运行 windeployqt...")
    windeployqt = QT_DIR / "bin" / "windeployqt.exe"
    cmd = [str(windeployqt), str(exe_path)]
    run_command(cmd)

    # --- 2. 获取 Clang/LLVM 工具链路径 ---
    # 通过 clang++ 的位置推断 LLVM 安装路径
    try:
        result = subprocess.run(["where", "clang++"], capture_output=True, text=True)
        if result.returncode != 0:
            raise FileNotFoundError("未找到 clang++")
        clangpp_path = Path(result.stdout.strip().splitlines()[0])
        llvm_bin_dir = clangpp_path.parent
        llvm_root_dir = llvm_bin_dir.parent  # 通常是 LLVM 安装根目录
    except Exception as e:
        print(f"错误：无法确定 LLVM 安装路径: {e}")
        sys.exit(1)

    print(f"检测到 LLVM 根目录: {llvm_root_dir}")

    # --- 3. 要复制的 Clang 运行时 DLL 列表 ---
    runtime_dlls = [
        "libc++.dll",
        "libunwind.dll",
        # 可选：如果使用 libc++abi
        # "libc++abi.dll",
    ]

    # --- 4. 复制 DLL ---
    for dll in runtime_dlls:
        src = llvm_bin_dir / dll
        dst = target_dir / dll
        if src.exists():
            import shutil
            print(f"复制 {src} -> {dst}")
            shutil.copy2(src, dst)
        else:
            print(f"警告：未找到 {src}，程序可能无法运行")

    print("部署完成")

def clean():
    if BUILD_DIR.exists():
        import shutil
        print(f"清理 {BUILD_DIR}")
        shutil.rmtree(BUILD_DIR)

# ------------------ 主函数 ------------------
def main():
    parser = argparse.ArgumentParser(description="Build Script")
    parser.add_argument("--clean", action="store_true")
    parser.add_argument("--release", action="store_true")

    args = parser.parse_args()
    build_type = "Release" if args.release else "Debug"

    if args.clean:
        clean()
        return

    clean()
    ensure_build_dir()
    configure(build_type)

    build(build_type)

    deploy()

    print("构建成功")

if __name__ == "__main__":
    main()
