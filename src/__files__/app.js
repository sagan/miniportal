
var rootEl = document.querySelector("#root");
var data = {
  isInitted: false,
  isAuthed: false,
};

(async () => {
  rootEl.innerHTML = render(data);

  let res = await fetch('/__auth__');
  data.isInitted = true;
  if( res.status != 200 ) {
    data.isAuthed = false;
  } else {
    data.isAuthed = true;
  }
  setData();
})();


function render(data) {
  return `
  <div>
    <h1>欢迎使用 Freedom Wi-Fi</h1><div>
    ${!data.isAuthed ?
      `<p>本 Wi-Fi 由 sagan 架设并维护。点击“立即连接”按钮开始上网。</p>` :
      `<p class="success">恭喜，您已经连接到自由的 Internet</p>`
    }
  </div>
  ${ data.isInitted ? 
    `<div>
    ${ !data.isAuthed ? 
      `<div>
        <button class="main-button" onclick="auth()" type="button">立即连接</button>
      </div>` :
      `<div>
        <p>
          <i>欢迎访问本 Wifi 维护者 sagan 的 <a href="https://sagan.me/">Blog</a>。
            如果您从本服务受益，请考虑捐助我们。捐助的用户将可以免验证使用本服务。
          </i>
          <br/>
          <div style="display: flex;">
            <div style="flex: 1">
              <h3>支付宝扫码捐赠</h3>
              <img src="/__files__/alipay-receive-money-with-text.png" /></div>
            <div style="flex: 1">
              <h3>Paypal 捐赠</h3>
            </div>
          </div>
        </p>
      </div>`
    }
  </div>` : 
  `<div>
    <p>Loading...
    </p>
  </div>`
  }
    <div>
      <h2>本 Wi-Fi 的优点和特色功能</h2>
      <ul>
        <li>2.4G 和 5G 双频，速率高达 300 + 866 Mbps</li>
        <li>自动国际网络加速，自由地访问任何网站</li>
        <li>集成 KMS 服务器，自动激活客户端的 Windows / Office 实例</li>
      </ul>
    </div>
  </div>`;
}

function setData(newData = {}) {
  Object.assign(data, newData);
  rootEl.innerHTML = render(data);
}

function auth(event) {
  var oReq = new XMLHttpRequest();
  oReq.open("POST", "/__auth__");
  oReq.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
  oReq.onreadystatechange = function() {
    if(oReq.readyState == XMLHttpRequest.DONE) {
      setData({isAuthed: true});
    }
  };
  oReq.send('username=test');
}