# 压力测试

使用webbench来进行压力测试


因为定时器部分代码有bug，所以下面的是注释掉定时器的情况下的测试：
![1](https://images.gitee.com/uploads/images/2021/1015/113248_8bd2f495_7524349.png "033aec26224325221f2e217951d8c4c.png")

当测试时间比较长时，会有部分数据发送失败：
![2](https://images.gitee.com/uploads/images/2021/1015/113930_132eec7b_7524349.png "308362103c45f5423841bd447229d71.png")

原因：没有定时器，服务器很多端口处于TIME_WAIt状态，这些状态不及时处理，导致服务器资源不足，无法建立连接
![输入图片说明](https://images.gitee.com/uploads/images/2021/1015/114130_a3d40c35_7524349.png "1634269227(1).png")

等待更新定时器代码后的压力测试，
