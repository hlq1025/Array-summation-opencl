# Array-summation-opencl
<<<<<<< HEAD
上一个版本，程序有bug。这个做了修改，对于数组个数小于等于2048的，均能得到正确结果。
=======
程序可以改为任意多的数求和，测试过数组个数为50和100时，已经通过。上一个版本只能对数组个数为2的整数次幂时正确求和。
>>>>>>> a0f7273889fc8e60342b0d85ad3a988330e9e25b
原来是同步不正确导致的错误，还不知道怎么修改。(opencl 不提供方法对所有的工作项同步，因此参考给的reduction原代码才可以实现大数组的求和算法。)
另外也搞懂了平台和设备的含义。
在我的电脑上有两个平台。intel和nvidia平台。其中nVidia平台上有设备GTX1060.
之前运行的程序都是在集显上运行的，程序已经改为在独显上运行了。