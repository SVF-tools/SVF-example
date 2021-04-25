## 1. Install SVF and its dependence (LLVM pre-built binary) via npm
```
npm i --silent svf-lib --prefix ${HOME}
```

## 2. Clone repository
```
git clone https://github.com/SVF-tools/SVF-example.git
```

## 3. Setup SVF environment and build your project 
```
source ./env.sh
```
cmake the project (`cmake -DCMAKE_BUILD_TYPE=Debug .` for debug build)
```
cmake .
make
```
## 4. Analyze a bc file using svf-ex executable
```
./bin/svf-ex example.ll
```
