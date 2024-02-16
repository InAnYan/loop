const { LanguageClient } = require('vscode-languageclient')

module.exports = {
  activate(context) {
    const executable = {
      command: 'loop_lsp.py',
      args: [],
    }

    const serverOptions = {
      run: executable,
      debug: executable,
    }

    const clientOptions = {
      documentSelector: [{
        scheme: 'file',
        language: 'loop',
      }],
    }

    const client = new LanguageClient(
      'loop',
      'Loop LSP',
      serverOptions,
      clientOptions
    )

    context.subscriptions.push(client.start())
  },
}