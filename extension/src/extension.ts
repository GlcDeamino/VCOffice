import * as vscode from 'vscode';
import * as path from 'path';
import * as fs from 'fs';

let wasmModule: any = null;

async function loadWasm() {
  if (wasmModule)  { return wasmModule; };

  const wasmPath = vscode.Uri.joinPath(
    vscode.extensions.getExtension('codiumdocuments')!.extensionUri,
    'out-wasm',
    'docx_unzip.js'
  );

  const wasmJsContent = await vscode.workspace.fs.readFile(wasmPath);
  const wasmJsCode = wasmJsContent.toString();

  // 注入 require 支持（Node.js 环境）
  const wrappedCode = `
    const require = globalThis.require || (() => {});
    ${wasmJsCode}
    globalThis.loadedWasmModule = Module;
  `;

  // 通过 eval 加载（在 Node.js 中允许）
  eval(wrappedCode);

  wasmModule = (globalThis as any).loadedWasmModule;
  await wasmModule.ready;
  return wasmModule;
}

export async function activate(context: vscode.ExtensionContext) {
  const disposable = vscode.commands.registerCommand('docx.open', async (uri: vscode.Uri) => {
    const docxPath = uri.fsPath;
    const cacheDir = docxPath + '.cache';

    // 确保目录存在
    if (!fs.existsSync(cacheDir)) {
      fs.mkdirSync(cacheDir, { recursive: true });
    }

    // 加载 WASM
    const mod = await loadWasm();

    // 调用 C++ 函数
    const result = mod.ccall(
      'wasm_unzip_docx',
      'number',
      ['string', 'string'],
      [docxPath, cacheDir]
    );

    if (result > 0) {
      vscode.window.showInformationMessage(`成功解压 ${result} 个文件到 ${cacheDir}`);
    } else {
      vscode.window.showErrorMessage('解压失败，请检查文件是否损坏');
    }
  });

  context.subscriptions.push(disposable);

  // 文件关联激活
  vscode.commands.executeCommand('setContext', 'docxExtensionReady', true);
}