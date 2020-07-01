## 1. clone repository
```
git clone https://github.com/SVF-tools/SVF-example.git
```

## 2. Install SVF via npm
```
npm install svf-lib
```

You are able to check your installed npm package and its path via command `npm list`
```
$ npm list
/home/samsung
└── svflib@1.0.0
```
Then your_path_to_SVF is `/home/samsung/node_modules/SVF/`.

your_path_to_LLVM is `/home/samsung/node_modules/llvm-10.0.0.obj`.
your_path_to_SVF_env `/home/samsung/node_modules/svf-lib/env.sh`.

## 3. Setup SVF environment
You are able to source SVF environment script to configure SVF environment. 
```
source /path/to/your/installed/npm/package/env.sh
```

## 4. cmake your project 
```
cmake .
make
```
