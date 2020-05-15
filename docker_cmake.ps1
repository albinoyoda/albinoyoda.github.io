Remove-Item ./interface/emscripten -Recurse -ErrorAction Ignore
mkdir -p interface/emscripten
docker run --rm -v ${PWD}:/src trzeci/emscripten /bin/bash -c "cd interface/emscripten &&  emconfigure cmake ../.. -DEMSCRIPTEN=True && emmake make"