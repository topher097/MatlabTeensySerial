Import("env")

env.Replace(
    UPLOADER="tycmd.exe",
    UPLOADCMD="$UPLOADER upload -B $UPLOAD_PORT $SOURCE"
)