rm -rf build
mkdir build
cp /home/terop/html/meshpage.org/web_page.data ./build/
cp /home/terop/html/meshpage.org/web_page.js ./build/
cp /home/terop/html/meshpage.org/web_page.wasm ./build/
cp /home/terop/html/meshpage.org/web_page.worker.js ./build/
cp /home/terop/html/meshpage.org/pthread-main.js ./build/
cp /home/terop/html/meshpage.org/web_page_lowmem.data ./build/
cp /home/terop/html/meshpage.org/web_page_lowmem.js ./build/
cp /home/terop/html/meshpage.org/web_page_lowmem.wasm ./build/
cp /home/terop/html/meshpage.org/web_page_lowmem.worker.js ./build/
cp /home/terop/html/meshpage.org/web_page_highmem.data ./build/
cp /home/terop/html/meshpage.org/web_page_highmem.js ./build/
cp /home/terop/html/meshpage.org/web_page_highmem.wasm ./build/
cp /home/terop/html/meshpage.org/web_page_highmem.worker.js ./build/
cp /home/terop/html/meshpage.org/web_page_nothreads.data ./build/
cp /home/terop/html/meshpage.org/web_page_nothreads.js ./build/
cp /home/terop/html/meshpage.org/web_page_nothreads.wasm ./build/
cp /home/terop/html/meshpage.org/web_page_nothreads.worker.js ./build/
cp /home/terop/html/meshpage.org/web_page_lowmem_nothreads.data ./build/
cp /home/terop/html/meshpage.org/web_page_lowmem_nothreads.js ./build/
cp /home/terop/html/meshpage.org/web_page_lowmem_nothreads.wasm ./build/
cp /home/terop/html/meshpage.org/web_page_lowmem_nothreads.worker.js ./build/
cp /home/terop/html/meshpage.org/web_page_nothreads_highmem.data ./build/
cp /home/terop/html/meshpage.org/web_page_nothreads_highmem.js ./build/
cp /home/terop/html/meshpage.org/web_page_nothreads_highmem.wasm ./build/
cp /home/terop/html/meshpage.org/web_page_nothreads_highmem.worker.js ./build/

cp gameapi_example.html build/gameapi_example.html
cp gameapi.js build/gameapi.js
(cd build;zip ../gameapi_display.zip *)
cp ./gameapi_example.html /home/terop/html/meshpage.org/
cp ./gameapi.js /home/terop/html/meshpage.org/
cp ./gameapi_display.zip /home/terop/html/meshpage.org/
echo "SUCCESS"
