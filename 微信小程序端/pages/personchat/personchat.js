// pages/personchat/personchat.js
Page({

  /**
   * 页面的初始数据
   */
  data: {
user:[
  {
name:'test1',
avatar:'/images/avatar.jpg',
status:'在线',
qian:'请和我聊聊天',
  },
  {
    name: 'test2',
    avatar: '/images/login.jpg',
    status: '在线',
    qian: '学习linux很快乐',
  },
  {
    name: 'test3',
    avatar: '/images/i.png',
    status: '在线',
    qian: '为美好发声',
  },
  




]
  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function (options) {


  },

  /**
   * 生命周期函数--监听页面初次渲染完成
   */
  onReady: function () {

  },

  /**
   * 生命周期函数--监听页面显示
   */
  onqian: function () {

  },

  /**
   * 生命周期函数--监听页面隐藏
   */
  onHide: function () {

  },

  /**
   * 生命周期函数--监听页面卸载
   */
  onUnload: function () {

  },

  /**
   * 页面相关事件处理函数--监听用户下拉动作
   */
  onPullDownRefresh: function () {

  },

  /**
   * 页面上拉触底事件的处理函数
   */
  onReachBottom: function () {

  },

  /**
   * 用户点击右上角分享
   */
  onShareAppMessage: function () {

  },

gotouserinfo:function(e){
  console.log(e.target.dataset.id)
 
  wx.navigateTo({
    url: '/pages/userinfo/userinfo?name=' + e.target.dataset.id,
  })  
},

gotochat:function(e){
  console.log(e.currentTarget.dataset.text)
  console.log(e.currentTarget.dataset.id)

  wx.navigateTo({
    url: '/pages/chat/chat?name=' + e.currentTarget.dataset.text + '&avatar=' + e.currentTarget.dataset.id,
  })
  
  
}

})