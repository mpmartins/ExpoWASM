# ExpoWASM

```bash
docker run -v $PWD:/work -w /work wasmsdk:v2 /build_wasm.sh
```

```bash
docker run \
-v ${PWD}/envoy.yaml:/etc/envoy.yaml \
-v ${PWD}/build/ExpoFilter.wasm:/etc/ExpoFilter.wasm \
-v ${PWD}/build/ExpoWorker.wasm:/etc/ExpoWorker.wasm \
-p 8000:8000 \
--entrypoint /usr/local/bin/envoy \
istio/proxyv2:1.7.0 -l info -c /etc/envoy.yaml --bootstrap-version 3
```
