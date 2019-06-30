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
      type: 'text',
      msg: 'hi~在linux中认识也是一种缘分，快来聊天吧~',
    },
    {
      type: 'text',
      msg: '我们来聊天吧~',
    },
    {
      type: 'text',
      msg: '：）',
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
  },


  getgetinputSEnd: function () {
    var input = this.data.inputdata;
    var that = this;
    var msgs = that.data.msgs;
    msgs.push({ msg: input, 'type': 'text' });
    //--------------------------------- 微信数据请求  

    wx.request({
      url: 'https://dashenbangmang.com/wxdjb/2019/sql.php',  //服务器更改
      data: {
        msg: input
      },
      header: {
        'Content-Type': 'application/json'
      },
      success: function (res) {
        msgs.push({ msg: res.data.content, 'type': res.data.msgType });
        that.setData({
          msgs: msgs,
          inputdata: '',
          scrollTop: that.data.scrollTop + 500
        });
        console.log(res.data)

      }
    })

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