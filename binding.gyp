{
  "targets": [
    {
      "target_name": "gdbm",
      "sources": [
        "gdbm.cc",
      ],
      "libraries": [
        "-lgdbm"
      ],
      "defines": [
        "NODE_GDBM_VERSION=\"<!@(node -e \"console.log(require('./package.json').version)\")\""
      ]
    }
  ]
}
