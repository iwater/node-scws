{
  "targets": [
    {
      "target_name": "nscws",
      "sources": [ "addon.cc", "scws.cc" ],
      "include_dirs" : ["/opt/scws/include", "node_modules/nan",  "<!(node -e \"require('nan')\")"],
      "libraries" : ["-lscws", "-L/opt/scws/lib"],
      "link_settings" : {
        "library_dirs" : ["/opt/scws/lib"]
      }
    }
  ]
}
