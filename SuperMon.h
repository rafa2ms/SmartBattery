/*


  OK, ya ready for some fun? HTML + CSS styling + javascript all in and undebuggable environment

  one trick I've learned to how to debug HTML and CSS code.

  get all your HTML code (from html to /html) and past it into this test site
  muck with the HTML and CSS code until it's what you want
  https://www.w3schools.com/html/tryit.asp?filename=tryhtml_intro

  No clue how to debug javascrip other that write, compile, upload, refresh, guess, repeat

  I'm using class designators to set styles and id's for data updating
  for example:
  the CSS class .tabledata defines with the cell will look like
  <td><div class="tabledata" id = "switch"></div></td>

  the XML code will update the data where id = "switch"
  java script then uses getElementById
  document.getElementById("switch").innerHTML="Switch is OFF";


  .. now you can have the class define the look AND the class update the content, but you will then need
  a class for every data field that must be updated, here's what that will look like
  <td><div class="switch"></div></td>

  the XML code will update the data where class = "switch"
  java script then uses getElementsByClassName
  document.getElementsByClassName("switch")[0].style.color=text_color;


  the main general sections of a web page are the following and used here

  <html>
    <style>
    // dump CSS style stuff in here
    </style>
    <body>
      <header>
      // put header code for cute banners here
      </header>
      <main>
      // the buld of your web page contents
      </main>
      <footer>
      // put cute footer (c) 2021 xyz inc type thing
      </footer>
    </body>
    <script>
    // you java code between these tags
    </script>
  </html>


*/

// note R"KEYWORD( html page code )KEYWORD"; 
// again I hate strings, so char is it and this method let's us write naturally

const char PAGE_MAIN[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en" class="js-focus-visible">

<title>Smart Battery v0</title>

  <style>
    <!--
	.center-container {
	  max-width: 1000px;
	  margin: 0 auto;
	  font-family: Arial;
	}-->
    table {
      <!-- position: relative; -->
	  position: relative;
      width: 100%;
	  
      border-spacing: 0px;
    }
    tr {
      border: 1px solid white;
      font-family: "Verdana", "Arial", sans-serif;
      font-size: 20px;
    }
    th {
      height: 20px;
      padding: 3px 15px;
      background-color: #343a40;
      color: #FFFFFF !important;
      }
    td {
      height: 20px;
       padding: 3px 15px;
    }
    .tabledata {
      font-size: 24px;
      position: relative;
      padding-left: 5px;
      padding-top: 5px;
      height:   25px;
      border-radius: 5px;
      color: #FFFFFF;
      line-height: 20px;
      transition: all 200ms ease-in-out;
      background-color: #00AA00;
    }
	<!--
    .fanrpmslider {
      width: 30%;
      height: 55px;
      outline: none;
      height: 25px;
    }-->
    .bodytext {
      font-family: "Verdana", "Arial", sans-serif;
      font-size: 24px;
      text-align: left;
      font-weight: light;
      border-radius: 5px;
      display:inline;
    }
    .navbar {
      width: 100%;
      height: 50px;
      margin: 0;
      padding: 10px 0px;
      background-color: #FFF;
      color: #000000;
      border-bottom: 5px solid #293578;
    }
    .fixed-top {
      position: fixed;
      top: 0;
      right: 0;
      left: 0;
      z-index: 1030;
    }
    .navtitle {
      float: left;
      height: 50px;
      font-family: "Verdana", "Arial", sans-serif;
      font-size: 50px;
      font-weight: bold;
      line-height: 50px;
      padding-left: 20px;
    }
   .navheading {
     position: fixed;
     left: 60%;
     height: 50px;
     font-family: "Verdana", "Arial", sans-serif;
     font-size: 20px;
     font-weight: bold;
     line-height: 20px;
     padding-right: 20px;
   }
   .navdata {
      justify-content: flex-end;
      position: fixed;
      left: 70%;
      height: 50px;
      font-family: "Verdana", "Arial", sans-serif;
      font-size: 20px;
      font-weight: bold;
      line-height: 20px;
      padding-right: 20px;
   }
    .category {
      font-family: "Verdana", "Arial", sans-serif;
      font-weight: bold;
      font-size: 32px;
      line-height: 50px;
      padding: 20px 10px 0px 10px;
      color: #000000;
    }
    .heading {
      font-family: "Verdana", "Arial", sans-serif;
      font-weight: normal;
      font-size: 28px;
      text-align: left;
    }
  
    .btn {
      background-color: #444444;
      border: none;
      color: white;
      padding: 10px 20px;
      text-align: center;
      text-decoration: none;
      display: inline-block;
      font-size: 16px;
      margin: 4px 2px;
      cursor: pointer;
    }
    .foot {
      font-family: "Verdana", "Arial", sans-serif;
      font-size: 10px;
      position: relative;
      height:   30px;
      text-align: center;   
      color: #AAAAAA;
      line-height: 10px;
    }
    .container {
      max-width: 1800px;
      margin: 0 auto;
    }
    table tr:first-child th:first-child {
      border-top-left-radius: 5px;
    }
    table tr:first-child th:last-child {
      border-top-right-radius: 5px;
    }
    table tr:last-child td:first-child {
      border-bottom-left-radius: 5px;
    }
    table tr:last-child td:last-child {
      border-bottom-right-radius: 5px;
    }
    
	/* -------------- CSS to existing styles ---------------- */
	form {
	  max-width: 400px;
	  <!-- margin: 0 auto; -->
	  left-margin = 100px;
	  font-family: Arial;
	}
	<!--
	.container {
	  width: 100%;
	}
	-->
	
	label {
	  display: block;
	  margin-bottom: 5px;
	  font-family: Arial;
	}
	
	label[for="uname"],
	label[for="psw"]{
	  font-family: Arial; /* Set the font for the username label */
	}

	input[type="text"],
	input[type="password"] {
	  width: 100%;
	  padding: 8px;
	  margin-bottom: 10px;
	  box-sizing: border-box;
	  font-family: Arial;
	}

	button {
	  background-color: #4caf50;
	  color: white;
	  padding: 10px 15px;
	  border: none;
	  border-radius: 5px;
	  cursor: pointer;
	  font-family: Arial;
	}

	.cancelbtn {
	  background-color: #ccc;
	  color: black;
	  font-family: Arial; /* Set the font for the username label */
	}

	.imgcontainer {
	  text-align: center;
	  margin: 24px 0 12px 0;
	}

	/* Responsive styles */
	@media (max-width: 767px) {
	  .tabledata,
	  th,
	  td {
		font-size: 18px;
		padding: 8px;
	  }
	}
    
  </style>

  <body style="background-color: #efefef" onload="process()">
  
    <header>
      <div class="navbar fixed-top">
          <div class="container">
            <div class="navtitle">Sensor Monitor</div>
            <div class="navdata" id = "date">mm/dd/yyyy</div>
            <div class="navheading">DATE</div><br>
            <div class="navdata" id = "time">00:00:00</div>
            <div class="navheading">TIME</div>
            
          </div>
      </div>
    </header>
	
	<!--<div class="center-container"> -->
		<main class="container" style="margin-top:70px;" >
			<div class="category">Sensor Readings</div>
			
			<div style="border-radius: 10px !important;">
				<table style="width:50%">
					<colgroup>
						<col span="1" style="background-color:rgb(230,230,230); width: 20%; color:#000000 ;">
						<col span="1" style="background-color:rgb(200,200,200); width: 15%; color:#000000 ;">
						<col span="1" style="background-color:rgb(180,180,180); width: 15%; color:#000000 ;">
					</colgroup>
					<col span="2"style="background-color:rgb(0,0,0); color:#FFFFFF">
					<col span="2"style="background-color:rgb(0,0,0); color:#FFFFFF">
					<col span="2"style="background-color:rgb(0,0,0); color:#FFFFFF">
					<tr>
						<th colspan="1"><div class="heading">Battery Cells</div></th>
						<th colspan="1"><div class="heading">Volts</div></th>
						<!--<th colspan="1"><div class="heading">Bits</div></th>-->
					</tr>
					<tr>
						<td><div class="bodytext">Cell 1</div></td>
						<td><div class="tabledata" id = "v0"></div></td>
						<!--<td><div class="tabledata" id = "b3"></div></td>-->
					</tr>
					
					<tr>
						<td><div class="bodytext">Cell 2</div></td>
						<td><div class="tabledata" id = "v1"></div></td>
						<!--<td><div class="tabledata" id = "b1"></div></td>-->
					</tr>
					
					<tr>
						<td><div class="bodytext">Cell 3</div></td>
						<td><div class="tabledata" id = "v2"></div></td>
					</tr>
					
					<tr>
						<td><div class="bodytext">Cell 4</div></td>
						<td><div class="tabledata" id = "v3"></div></td>
					</tr>
					
					
					<!--
					<tr>
						<td><div class="bodytext">Digital switch</div></td>
						<td><div class="tabledata" id = "switch"></div></td>
					</tr>-->
				</table>
			</div>
			<br>
			
			
			
			<div class="bodytext">Digital switch
				<div class="btn" id = "switch"></div>
			</div>
		
			
			<div class="category">Sensor Controls </div> 
			<br>
			<div class="bodytext">LED </div>
			<button type="button" class = "btn" id = "btn0" onclick="ButtonPress0()">Toggle</button>
			</div>
			<br>
			<div class="bodytext">Switch</div>
			<button type="button" class = "btn" id = "btn1" onclick="ButtonPress1()">Toggle</button>
			</div>
			<br>
			<br>
			<!-- This is a comment -->
			<!--<div class="bodytext">Fan Speed Control (RPM: <span id="fanrpm"></span>)</div>-->
			<br>
			<!--<input type="range" class="fanrpmslider" min="0" max="255" value = "0" width = "0%" oninput="UpdateSlider(this.value)"/>-->
			
			<!--<div class="tabledata" id = "b0"></div><br>
			<div class="tabledata" id = "b1"></div><br>-->
		</main>
		
        <br>
            <div class="category">Wi-fi Connection</div>
        <br>

        <p id="dynamicText_Status">No stored Wi-Fi :(</p>
        <p id="dynamicText_Uname"> </p>
        
        <button type="button" class = "btn" id = "btn_altwifi" onclick="ButtonPress_altwifi()">Reconect</button> <!--onclick=-->
        <button type="button" class = "btn" id = "btn_neuwifi" onclick="ButtonPress_neuwifi()">New Wi-Fi</button>  <!--onclick="ButtonPress_neuwifi()-->
        <br>
        <br>

		<form  action="/advices" method="post" style="margin-left:30px;" id = "myForm"> <!-- a -->

			<div class="container">
				<label for="uname"><b>Username</b></label>
				<input type="text" placeholder="Enter Username" name="uname" required>

				<label for="psw"><b>Password</b></label>
				<input type="password" placeholder="Enter Password" name="psw" required>

				<button type="submit">Login</button>

				<label>
				  <input type="checkbox" checked="checked" name="remember"> Remember me
				</label>
			</div>

			<div class="container" style="background-color:#f1f1f1">
				<button type="button" class="cancelbtn">Cancel</button>
			</div>
		</form>
	<!-- </div> -->

  <footer div class="foot" id = "temp" >ESP32 Web Page Creation and Data Update Demo</div></footer>
  
  </body>


  <script type = "text/javascript">
  
    // global variable visible to all java functions
    var xmlHttp=createXmlHttpObject();

    // function to create XML object
    function createXmlHttpObject(){
      if(window.XMLHttpRequest){
        xmlHttp=new XMLHttpRequest();
      }
      else{
        xmlHttp=new ActiveXObject("Microsoft.XMLHTTP");
      }
      return xmlHttp;
    }

    // function to handle button press from HTML code above
    // and send a processing string back to server
    // this processing string is use in the .on method
    function ButtonPress0() {
      var xhttp = new XMLHttpRequest(); 
      var message;
      // if you want to handle an immediate reply (like status from the ESP
      // handling of the button press use this code
      // since this button status from the ESP is in the main XML code
      // we don't need this
      // remember that if you want immediate processing feedbac you must send it
      // in the ESP handling function and here
      /*
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          message = this.responseText;
          // update some HTML data
        }
      }
      */
       
      xhttp.open("PUT", "BUTTON_0", false);
      xhttp.send();
    }


    // function to handle button press from HTML code above
    // and send a processing string back to server
    // this processing string is use in the .on method
    function ButtonPress1() {
      var xhttp = new XMLHttpRequest(); 
      /*
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("button1").innerHTML = this.responseText;
        }
      }
      */
      xhttp.open("PUT", "BUTTON_1", false);
      xhttp.send(); 
    }

    function ButtonPress_altwifi() {
        var xhttp = new XMLHttpRequest();

        // Open and send the request
        xhttp.open("PUT", "advices", true);
        xhttp.send();

        setTimeout(function() {
          //your code to be executed after 1 second
        }, 50);

        window.location.href = '/advices';
    }

    //setVisibility("myForm", false);
    function setVisibility(klass, visibility) {
        var element = document.getElementById(klass);

        // Toggle visibility
        if (visibility) { //button.style.display === "none"
            element.style.display = "block"; // or "inline" depending on your styling
        } else {
            element.style.display = "none";
        }
    }

    function ButtonPress_neuwifi() {
      //setVisibility("myForm", true);
      
      form = document.getElementById("myForm");
      if (form.style.display == "none") {
        setVisibility("myForm", true);
      } else {
        setVisibility("myForm", false);
      }
      

      /*
      var xhttp = new XMLHttpRequest(); 
      
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("button1").innerHTML = this.responseText;
        }
      }
      
      xhttp.open("PUT", "BUTTON_1", false);
      xhttp.send(); 
      */
    }
    
    function UpdateSlider(value) {
      var xhttp = new XMLHttpRequest();
      // this time i want immediate feedback to the fan speed
      // yea yea yea i realize i'm computing fan speed but the point
      // is how to give immediate feedback
	  /*
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          // update the web based on reply from  ESP
          document.getElementById("fanrpm").innerHTML=this.responseText;
        }
      }
	  */
      // this syntax is really weird the ? is a delimiter
      // first arg UPDATE_SLIDER is use in the .on method
      // server.on("/UPDATE_SLIDER", UpdateSlider);
      // then the second arg VALUE is use in the processing function
      // String t_state = server.arg("VALUE");
      // then + the controls value property
      xhttp.open("PUT", "UPDATE_SLIDER?VALUE="+value, true);
      xhttp.send();
    }

    

    var ssid = "";
    var pwd = "";
    var connect = "No stored Wi-Fi :(";
    setVisibility("btn_altwifi", false);

    // function to handle the response from the ESP
    function response(){
      var message;
      var barwidth;
      var currentsensor;
      var xmlResponse;
      var xmldoc;
      var dt = new Date();
      var color = "#e8e8e8";
      var result = [0,0];
      // get the xml stream
      xmlResponse=xmlHttp.responseXML;
  
      // get host date and time
      document.getElementById("time").innerHTML = dt.toLocaleTimeString();
      document.getElementById("date").innerHTML = dt.toLocaleDateString();
	  
	  for (let i = 0; i < 4; i++){
		xmldoc = xmlResponse.getElementsByTagName("V"+i); //volts for A0
		message = xmldoc[0].firstChild.nodeValue;
		  
		result = voltagerange(message);
		barwidth = result[0];
		color = result[1];
		
		document.getElementById("v"+i).innerHTML=message;
		document.getElementById("v"+i).style.width=(barwidth+"%");
		// you can set color dynamically, maybe blue below a value, red above
		document.getElementById("v"+i).style.backgroundColor=color;
		//document.getElementById("v0").style.borderRadius="5px";
	  }
      
      xmldoc = xmlResponse.getElementsByTagName("LED");
      message = xmldoc[0].firstChild.nodeValue;
  
      if (message == 0){
        document.getElementById("btn0").innerHTML="Turn ON";
      }
      else{
        document.getElementById("btn0").innerHTML="Turn OFF";
      }
         
      xmldoc = xmlResponse.getElementsByTagName("SWITCH");
      message = xmldoc[0].firstChild.nodeValue;
      document.getElementById("switch").style.backgroundColor="rgb(200,200,200)";
      // update the text in the table
      if (message == 0){
        document.getElementById("switch").innerHTML="Switch is OFF";
        document.getElementById("btn1").innerHTML="Turn ON";
        document.getElementById("switch").style.color="#0000AA"; 
      }
      else {
        document.getElementById("switch").innerHTML="Switch is ON";
        document.getElementById("btn1").innerHTML="Turn OFF";
        document.getElementById("switch").style.color="#00AA00";
      }

      xmldoc = xmlResponse.getElementsByTagName("WIFIU");
      message = xmldoc[0].firstChild.nodeValue;

      if (message != ""){
        connect = "Stored Wi-Fi: ";
        ssid = message;
        setVisibility("btn_altwifi",true);
        //setVisibility("myForm", false);

      }else{
        ssid = "";
        pwd = "";
        connect = "No stored Wi-Fi :(";
        setVisibility("btn_altwifi", false);        
      }

      xmldoc = xmlResponse.getElementsByTagName("WIFIP");
      message = xmldoc[0].firstChild.nodeValue;

      document.getElementById("dynamicText_Status").innerHTML= connect;
      document.getElementById("dynamicText_Uname").innerHTML= ssid;
    }
  
    // general processing code for the web page to ask for an XML steam
    // this is actually why you need to keep sending data to the page to 
    // force this call with stuff like this
    // server.on("/xml", SendXML);
    // otherwise the page will not request XML from the ESP, and updates will not happen
    function process(){
     
     if(xmlHttp.readyState==0 || xmlHttp.readyState==4) {
        xmlHttp.open("PUT","xml",true);
        xmlHttp.onreadystatechange=response;
        xmlHttp.send(null);
      }       
        // you may have to play with this value, big pages need more porcessing time, and hence
        // a longer timeout
        setTimeout("process()",100);
    }
	
	function voltagerange(message){
	
	  var w_px = 100;
	  var v_max = 3;
	  var barwidth = (w_px * message)/v_max; // x pixels = (w_px * Y volts )/5 volts
	  var color;
	  
	  if (message > (0.75 * v_max) ){
		color = "#25cf25"; // green
      }
	  else{
		if((message > (0.35 * v_max) )){
			color = "#ffed2b"; // yellow
		}
		else{
			color = "#aa0000"; // red
		} 
      }
	  
	  return [barwidth, color];
	}
  
  </script>

</html>
)=====";


const char PAGE_ADVICES[] = R"=====(
 <!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <style>
    body {
      font-family: 'Arial', sans-serif;
      background-color: #f0f0f0;
      margin: 0;
      padding: 0;
      box-sizing: border-box;
    }

    header {
      background-color: #333;
      color: white;
      text-align: center;
      padding: 10px;
	  font-size: 50px;
    }

    main {
      padding: 20px;
      text-align: center;
	  font-size: 30px;
    }

    footer {
      background-color: #333;
      color: white;
      text-align: center;
      padding: 10px;
      position: fixed;
      bottom: 0;
      width: 100%;
	  font-size: 10px;
    }
  </style>
  <title>Connecting to local Wi-Fi</title>
</head>
<body>
  <header>
    We're almost there...
  </header>

  <main>
  
  <br/>
    • Please, check if the green LED in the battery has stopped to blink.<br/><br/>
    • A steadly light indicates that the connection was successful.<br/><br/>
    • In case of failure, the LED will turn off and a new attempt will be requested.<br/>
  </main>

  <footer>
    © 2023 xyz inc.
  </footer>
</body>
</html>

)=====";
