# Array-summation-opencl

数组大小为5000时是可以得到正确结果的，6000时错误。
原来是同步不正确导致的错误，还不知道怎么修改。(opencl 不提供方法对所有的工作项同步，因此参考给的reduction原代码才可以实现大数组的求和算法。)
另外也搞懂了平台和设备的含义。
在我的电脑上有两个平台。intel和nvidia平台。其中nVidia平台上有设备GTX1060.
之前运行的程序都是在集显上运行的，程序已经改为在独显上运行了。

