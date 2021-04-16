If(!(test-path website/emscripten))
{
      mkdir -p website/emscripten
}
docker run --rm -v ${PWD}:/src trzeci/emscripten /bin/bash -c "cd website/emscripten &&  emcmake cmake ../.. -DEMSCRIPTEN=True && emmake make"

# Complie docker container
# docker build -t rzeci/emscripten .