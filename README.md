## 1. clone repository
```
git clone https://github.com/SVF-tools/SVF-example.git
```

## 2. Install SVF via npm
```
npm install svf-lib
```

## 3. Setup SVF environment
You are able to source SVF environment script to configure SVF environment. 
```
source ./env.sh
```
## 4. cmake your project 
```
cmake .
```
```
make
```
## 5. Analyze a bc file using svf-ex executable
```
./bin/svf-ex example.bc
```
