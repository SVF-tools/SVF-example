## 1. Clone repository
```
git clone https://github.com/SVF-tools/SVF-example.git
```

## 2. Install SVF via npm
```
npm install svf-lib
```

## 3. Setup SVF environment and build your project 
```
source ./env.sh
```
cmake the project
```
cmake .
make
```
## 4. Analyze a bc file using svf-ex executable
```
./bin/svf-ex example.bc
```
