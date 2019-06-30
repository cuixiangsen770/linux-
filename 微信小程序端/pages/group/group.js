// pages/group/group.js
Page({

  /**
   * 页面的初始数据
   */
  data: {
group:[
{
groupid:'请稍等',
groupname:'稍等',
host:'陪你等',
groupinfo:'稍等一下，正在加载',
chatnum:999,
usernum:999,
avatar:'/images/avatar.jpg',
},


]
  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function (options) {
var that = this
wx.request({
  url: 'http://212.64.122.38/grouplist',
  success(res){
    console.log(res.data)
    that.setData({
      group:res.data
    })
  }
})
  },

  /**
   * 生命周期函数--监听页面初次渲染完成
   */
  onReady: function () {

  },

  /**
   * 生命周期函数--监听页面显示
   */
  onShow: function () {

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

  }
})