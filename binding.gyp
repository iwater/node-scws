{
  "targets": [
    {
      "target_name": "nscws",
      "sources": [ "scws.cc" ],
      "include_dirs" : ["/opt/scws/include", "<!(node -e \"require('nan')\")"],
      "libraries" : ["-lscws", "-L/opt/scws/lib"],
      "link_settings" : {
        "library_dirs" : ["/opt/scws/lib"]
      }
    }
  ]
}
