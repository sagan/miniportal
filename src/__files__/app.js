var rootEl = document.querySelector("#root");
var data = {
  sitename: "Freedom Wi-Fi",
  owner: "sagan",
  ownerUrl: "https://sagan.me/",
  ownerTwitter: "",
  paypalCode: ``,
  isInitted: false,
  isAuthed: false
};

if (window.CONFIG) {
  Object.keys(data).forEach(function(key) {
    if (typeof window.CONFIG[key] != "undefined") {
      data[key] = window.CONFIG[key];
    }
  });
}

(function() {
  rootEl.innerHTML = render(data);
  fetch("/__auth__").then(function(res) {
    data.isInitted = true;
    if (res.status != 200) {
      data.isAuthed = false;
    } else {
      data.isAuthed = true;
    }
    setData();
  });
})();

function render(data) {
  return `
  <div>
    <h1>欢迎使用 ${data.sitename}</h1>
    <div>
      ${
        !data.isAuthed
          ? `<p>本 Wi-Fi 由 ${
              data.owner
            } 架设并维护。点击“立即连接”按钮开始上网。</p>`
          : `<p class="success">恭喜，您已经连接到自由的 Internet</p>`
      }
    </div>
  ${
    data.isInitted
      ? `
    <div>
      ${
        !data.isAuthed
          ? `
      <div>
        <button class="main-button" onclick="auth()" type="button">立即连接</button>
      </div>`
          : `
      <div>
        <p>
          <i>欢迎访问本 Wifi 维护者 ${data.owner} 的 <a href="${
              data.ownerUrl
            }">个人网站</a>
            ${
              data.ownerTwitter
                ? ` 或 <a href="https://twitter.com/${
                    data.ownerTwitter
                  }">Twitter</a>`
                : ""
            }。
            如果您从本服务受益，请考虑捐助我们。捐助的用户将可以免验证使用本服务。
          </i>
          <br/>
          <div style="display: flex;">
            <div style="flex: 1">
              <h3>支付宝扫码捐赠</h3>
              <img src="/__files__/alipay-donate.png" /></div>
            <div style="flex: 1">
              <h3>Paypal 捐赠</h3>
              ${data.paypalCode}
            </div>
          </div>
        </p>
      </div>`
      }
    </div>`
      : `
    <div>
      <p>Loading...</p>
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

function setData(newData) {
  Object.keys(newData || {}).forEach(function(key) {
    data[key] = newData[key];
  });
  rootEl.innerHTML = render(data);
}

function auth(event) {
  fetch("/__auth__", {
    method: "POST",
    headers: {
      "Content-Type": "application/x-www-form-urlencoded"
    },
    body: "username=test"
  }).then(
    function(res) {
      if (res.status != 200) throw new Error(`Server Error ${res.status}`);
      setData({ isAuthed: true });
    },
    function(err) {
      alert(`出错了: ${err}`);
    }
  );
}
