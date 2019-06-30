const app = getApp();
Page({
  data: {
    imgUrls: [
      {
        link: 'a',
        url: '/images/login.png'
      }
    ],
    scrollTop: 1000,
    indicatorDots: false,
    autoplay: false,
    interval: 5000,
    duration: 1000,
    msgs: [{
      msg: 'hi~在linux中认识也是一种缘分，快来聊天吧~',
      send:'测试'
    },
    {
      msg: '我们来聊天吧~',
      send:'顺其自然'
    },
    {
      msg: '：）',
      send:'测试'
    },
    ],
    inputdata: '',
    userInfo: {},
    avatar:'',
    name:''
  },

  onLoad: function (e) {
    console.log(e)
    this.setData({
     avatar:e.avatar,
     name:e.name,
    })
   // console.log('onLoad test');
    console.log(this.data.msgs);
    if (app.globalData.userInfo) {
      this.setData({
        userInfo: app.globalData.userInfo,
        hasUserInfo: true
      })
    } else if (this.data.canIUse) {
      // 由于 getUserInfo 是网络请求，可能会在 Page.onLoad 之后才返回
      // 所以此处加入 callback 以防止这种情况
      app.userInfoReadyCallback = res => {
        this.setData({
          userInfo: res.userInfo,
          hasUserInfo: true
        })
      }
    } else {
      // 在没有 open-type=getUserInfo 版本的兼容处理
      wx.getUserInfo({
        success: res => {
          app.globalData.userInfo = res.userInfo
          this.setData({
            userInfo: res.userInfo,
            hasUserInfo: true
          })
        }
      })
    }



    var that=this
    wx.request({
      url: 'http://212.64.122.38/personchat',
      data:{
        name:that.data.name,
        my:app.globalData.userInfo.nickName,

      },
      success(res){
         console.log(res.data)
        that.setData({
          msgs: res.data
        })
  
      },
    
    })
    console.log(that.data.msgs)
  },


  getgetinputSEnd: function () {
    var input = this.data.inputdata;
    var that = this;

    //--------------------------------- 微信数据请求  
  
    wx.request({
      url: 'http://212.64.122.38/addpersonchat',  //服务器更改
      data: {
        msg: input,
        rev:that.data.name,
        send:app.globalData.userInfo.nickName,
      },
      header: {
        'Content-Type': 'application/json'
      },
      success: function (res) {
        console.log(res.data)
      /*  msgs.push({ msg: res.data.message, 'send': res.data.send });
        that.setData({
          msgs: msgs,
          inputdata: '',
          scrollTop: that.data.scrollTop + 500
        });
        console.log(res.data)
       */
      }
    })


    wx.request({
      url: 'http://212.64.122.38/personchat',
      data: {
        name: that.data.name,
        my: app.globalData.userInfo.nickName,

      },
      success(res) {
        console.log(res.data)
        that.setData({
          msgs: res.data
        })

      },

    })
//    console.log(that.data.msgs)


  },
  setInputValue: function (e) {
    console.log(e.detail);
    this.data.inputdata = e.detail.value;
  },


  1: function () {
    wx.redirectTo({
      url: '/pages/0/index',
    })
  },
  2: function () {
    wx.redirectTo({
      url: '/pages/index/index',
    })
  },
  onShareAppMessage: function () {
    // 用户点击右上角分享
    return {
      title: '有人@我,这是地大人的党建信息小程序', // 分享标题
      desc: '有人@我,这是地大人的党建信息小程序', // 分享描述
      path: '/pages/sql/sql' // 分享路径
    }
  },
  a: function () {
    wx.navigateTo({
      url: '../standard/standard',
      success: function (res) { },
      fail: function (res) { },
      complete: function (res) { },
    })
  },
  qing: function () {
    wx.reLaunch({
      url: '../sql/sql',
    })
  }
})  