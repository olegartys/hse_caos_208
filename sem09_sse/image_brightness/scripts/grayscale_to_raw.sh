ffmpeg -vcodec png -i "${1}" -vcodec rawvideo -f rawvideo -pix_fmt gray "${2}"
