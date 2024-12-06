String homePagePart1 = F(R"=====(<!DOCTYPE html> 
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1, viewport-fit=cover">
<title>Servo Control WebServer</title>
<meta http-equiv="refresh" content="3">
<style>
body { background-color: DodgerBlue; }
.flex-Container{ display: flex; flex-direction: column; align-items: center; }
h1{ font-size: 40px; font-family: Arial; color: navy; text-align: center; }
p{ font-size: 25px; font-family: Arial; color: navy; text-align: center; }
table { font-size: 25px; padding: 8px; text-align: left; border-bottom: 1px solid #ddd; }
</style>
</head>
<body>
<div class="flex-Container">
<h1>Servo Control Website</h1>
<p>Welcome to my website where you can control the servo.</p>
<button onclick="handleTurnOn()">Trigger Servo</button>
<script>
    // Function to trigger the servo
    function handleTurnOn() {
      fetch("/turnon").then(response => response.text()).then(alert);
    }
  </script>

 <button onclick="pirON()">PIR ON</button>
<button onclick="pirOFF()">PIR OFF</button>
<script>
  function pirON() {
    fetch("/piron").then(response => response.text()).then(alert);
  }
  function pirOFF() {
    fetch("/piroff").then(response => response.text()).then(alert);
  }
</script>


  <p>Current Servo Status: %s</p>
</div>

</body>
</html>)=====");

String homePagePart2 = F(R"=====(</td><td></td></tr></table></div></body></html>)=====");
 