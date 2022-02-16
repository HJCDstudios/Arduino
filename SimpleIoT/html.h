#ifndef _HTML_
#define _HTML_ 1
String getIndexHtml() {
  return F(R"rawliteral(
<!DOCTYPE html>
<html lang="en">
  <head>
    <title>Simple IoT</title>
    <meta name="viewport" content="initial-scale=1,maximum-scale=3"/>
    <style type="text/css">
      h1,h2,h3,h4,h5,h6,li {
        color: rgb(60,60,60);
        margin:2px;
        font-family: Sans-Serif;
      }
      body {
        margin:0;
      }
      table {
        border-color: rgb(60,60,60);
        border-collapse: collapse;
        border-width: 2px;
        width: auto;
        max-width: 300px;
      }
      td {
        text-align: center;
      }
      button {
        border: 0px;
        padding: 5px;
        background-color: grey;
        color: white;
        border-radius: 5px;
        font-size:19px;
      }
      input[type="number"] {
        font-size: 19px;
        padding: 5px;
        border-radius: 3px;
      }
    </style>
  </head>
  <body onload="OnStart()">
    <noscript>
      <h1>This page will not work</h1>
    </noscript>
    <main style="display:none" id="#0">
      <center>
        <h1>Welcome to simple IoT</h1>
        <hr/>
        <h1>Dashboard</h1>
      </center>
      <h3>DHT11 Sensor:</h3>
      <ul>
        <li id="#1">Temperature: 0C</li>
        <li id="#2">Humidity: 0%</li>
      </ul>
      <h3>HC-SR04 Sensor:</h3>
      <ul>
        <li id="#5">Distance (cm): 0</li>
        <li id="#5.1">Distance (in): 0</li>
      </ul>
      <h3 id="#4">Uptime: </h3><br/>
      <input type="number" id="hzs" placeholder="buzzer hz (1 - 2000)"/><button onselectstart="return false;" ontouchstart="sound(true)" ontouchend="sound(false)">Play</button>
      <hr/><br/>
      <center>
        <h1>Pins States</h1>
        <table style="height:100%" border>
          <thead>
            <tr>
              <td colspan="2">
                <h2>Digital</h2>
              </td>
              <td colspan="2">
                <h2>Analog</h2>
              </td>
            </tr>
          </thead>
          <tbody>
            <tr>
              <td>
                <h3>Pin</h3>
              </td>
              <td>
                <h3>State</h3>
              </td>
              <td>
                <h3>Pin</h3>
              </td>
              <td>
                <h3>State</h3>
              </td>
            </tr>
          </tbody>
          <tfoot id="#3">
            <tr>
              <td colspan="4">
                <h4>No Data</h4>
              </td>
            </tr>
          </tfoot>
        </table>
      </center>
      <script type="text/javascript">
      var ws = null;
      var digitalpinnames = [],
      analogpinnames = [];
      function OnStart() {
        try {
          ws = new WebSocket("ws://"+location.hostmame+":81/",["arduino"]);
          ws.onopen = function() {
            id("#0").style.display = "block";
          };
          ws.onclose = function() {
            if (confirm("The server closed\nWould you like to reload")) location.reload()
          };
          ws.onmessage = function(obj) {
            obj = JSON.parse(obj.data);
            if (obj.type == "init") {
              digitalpinnames = obj.digitalPins;
              analogpinnames = obj.analogPins;
            } else if (obj.type == "update-pin") {
              loadpins(obj);
            } else if (obj.type == "update-dht11") {
              id("#1").innerHTML = "Temperature: "+obj.temperature+"C";
              id("#2").innerHTML = "Humidity: "+obj.humidity+"%";
            } else if (obj.type == "update") {
              id("#4").innerHTML = "Uptime: "+obj.uptime[0]+" second"+(obj.uptime[0]>1?"s":"")+", "+obj.uptime[1]+" minute"+(obj.uptime[1]>1?"s":"")+" and "+obj.uptime[2]+" hour"+(obj.uptime[2]>1?"s":"");
            } else if (obj.type == "update-hcsr04") {
              id("#5").innerHTML = "Distance (cm): "+obj.cm;
              id("#5.1").innerHTML = "Distance (in): "+obj.in;
            }
          };
        } catch(e) {
          alert(e);
          if (confirm("Would you like to reload")) location.reload();
        }
      }
      function loadpins(obj) {
        id("#3").innerHTML = "";
        var len = obj.digitalPinStates.length>=obj.analogPinStates.length?obj.digitalPinStates.length:obj.analogPinStates.length;
        for(var i=0;i<len;i++) {
          var d = obj.digitalPinStates[i]?obj.digitalPinStates[i]:"";
          var a = obj.analogPinStates[i]?obj.analogPinStates[i]:"";
          var d1 = digitalpinnames[i]?digitalpinnames[i]:"";
          var a1 = analogpinnames[i]?analogpinnames[i]:"";
          id("#3").innerHTML += "<tr><td><h4>"+d1+"</h4></td><td><h4>"+d+"</h4></td><td><h4>"+a1+"</h4></td><td><h4>"+a+"</h4></td></tr>";
        }
      }
      function id(idname) {
        return document.getElementById(idname);
      }
      function sound(bo) {
        if (bo) {
          var val = parseInt(id("hzs").value);
          if (val < 0 || val > 2000) return alert("Error While Using Buzzer\nMin: 0, Max: 2000")
          ws.send(JSON.stringify({
            "type":"activate",
            "hz":val
          }));
        } else {
          ws.send(JSON.stringify({
            "type":"disable"
          }));
        }
      }
      </script>
    </main>
  </body>
</html>
)rawliteral");
}
#endif