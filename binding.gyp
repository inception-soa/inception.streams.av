{
    "targets": [
        {
            "target_name": "inception.streams.av",
            "sources": ["src/inception.streams.av.c"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")"
            ]
        }
    ],
}
