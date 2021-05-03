'use strict'

// C library API
const ffi = require('ffi-napi');

// Express App (Routes)
const express = require("express");
const app = express();
const path = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());
app.use(express.static(path.join(__dirname + '/uploads')));

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');
const { send } = require('process');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/', function (req, res) {
  res.sendFile(path.join(__dirname + '/public/index.html'));
});

// Send Style, do not change
app.get('/style.css', function (req, res) {
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname + '/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js', function (req, res) {
  fs.readFile(path.join(__dirname + '/public/index.js'), 'utf8', function (err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, { compact: true, controlFlowFlattening: true });
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function (req, res) {
  if (!req.files) {
    return res.status(400).send('No files were uploaded.');
  }

  let uploadFile = req.files.uploadFile;

  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function (err) {
    if (err) {
      return res.status(500).send(err);
    }

    res.redirect('/');
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function (req, res) {
  fs.stat('uploads/' + req.params.name, function (err, stat) {
    if (err == null) {
      res.sendFile(path.join(__dirname + '/uploads/' + req.params.name));
    } else {
      console.log('Error in file downloading route: ' + err);
      res.send('');
    }
  });
});

//******************** Your code goes here ********************

// FFI LIBRARY
let GPXparser = ffi.Library('./libgpxparser.so', {
  "gpxfiletoJSON": ['string', ['string']],
  "gpxroutetoJSON": ['string', ['string']],
  "gpxtracktoJSON": ['string', ['string']],
  "GPXViewtoJSON": ['string', ['string']],
  "createGPXFileFromJSON": ['string', ['string', 'string', 'double', 'string', 'int', 'int', 'int']],
  "addRouteFromJSON": ['void', ['string', 'string', 'string', 'double', 'double', 'string', 'string']],
  "addWaypointFromJSON": ['string', ['string', 'string', 'string', 'double', 'double']],
  "routesBetweenFromJSON2": ['string', ['string', 'float', 'float', 'float', 'float', 'float']],
  "TracksBetweenFromJSON2": ['string', ['string', 'float', 'float', 'float', 'float', 'float']],
  "renameRoute": ['string', ['string', 'string', 'string']],
  "renameTrack": ['string', ['string', 'string', 'string']],
  "ShowRouteData": ['string', ['string', 'string']],
  "ShowTrackData": ['string', ['string', 'string']],
  "findBetween": ['string', ['string', 'float', 'float', 'float', 'float', 'float']],
  "waypointsFromJSON": ['string', ['string', 'string']],
  "routeLenFromJSON":['float',['string','string']]
});

// Sample endpoint
app.get('/endpoint1', function (req, res) {
  let retStr = req.query.stuff + " " + req.query.junk;
  res.send({
    stuff: retStr
  });
});

// FILE MANIPULATION
app.get('/getFileList', function (req, res) {
  let obj = new Array();
  fs.readdirSync('./uploads', function (e) { }).forEach(filename => {
    if (filename.substring(filename.length - 4) == ".gpx") {
      let results = GPXparser.gpxfiletoJSON(path.join(__dirname + '/uploads/' + filename));
      results = results.replace("}", ",\"filename\":\"" + filename + "\"}");
      obj.push(results);
    }
  });
  res.send(obj);
});
app.get('/updateGPX', function (req, res) {

  if (!req.query.value.substring(req.query.value.length - 4) == ".gpx") {
    res.send("");
  }

  let list = GPXparser.GPXViewtoJSON(path.join(__dirname + '/uploads/' + req.query.value));

  res.send(list);
});
app.get('/createGPXFile', function (req, res) {
  let obj = new Array();
  fs.readdirSync('./uploads', function (e) { }).forEach(filename => {
    if (filename.substring(filename.length - 4) == ".gpx") {
      let results = GPXparser.gpxfiletoJSON(path.join(__dirname + '/uploads/' + filename));
      results = results.replace("}", ",\"filename\":\"" + filename + "\"}");
      obj.push(results);
    }
  });
  if (!req.query.file.substring(req.query.file.length - 4) == ".gpx") {
    res.send("");
  }
  GPXparser.createGPXFileFromJSON(path.join(__dirname + '/uploads/' + req.query.file), req.query.nspace, req.query.ver, req.query.cre, req.query.wpt, req.query.rte, req.query.trk);

  res.send(obj);
});

// TABLE MANIPULATION
app.get('/AddRoute', async function (req, res) {
  if (!req.query.value.substring(req.query.value.length - 4) == ".gpx") {
    res.send("");
  }
  GPXparser.addRouteFromJSON(path.join(__dirname + '/uploads/' + req.query.value), req.query.rte, req.query.wpt, req.query.lat, req.query.lon, req.query.dName, req.query.dValue);
  await updateAdd(req.query.rte,req.query.value,path.join(__dirname + '/uploads/' + req.query.value),req.query.lat,req.query.lon,req.query.wpt,0);
  res.send("");
});

app.get('/AddWaypoint', async function (req, res) {
  if (!req.query.value.substring(req.query.value.length - 4) == ".gpx") {
    res.send("");
  }
  let string = GPXparser.addWaypointFromJSON(path.join(__dirname + '/uploads/' + req.query.value), req.query.rte, req.query.wpt, req.query.lat, req.query.long);
  // if(data.check == true) 
  let data = JSON.parse(string);
  if(data.check == true) await updateWpt(req.query.rte,path.join(__dirname + '/uploads/' + req.query.value),req.query.value);
  // await updateAdd(req.query.rte,req.query.value,path.join(__dirname + '/uploads/' + req.query.value),req.query.lat,req.query.lon,req.query.wpt,1);
  res.send(string);
});
app.get('/findPath', function (req, res) {
  if (!req.query.value.substring(req.query.value.length - 4) == ".gpx") {
    res.send("");
  }
  let result = GPXparser.findBetween(path.join(__dirname + '/uploads/' + req.query.value), req.query.sLat, req.query.eLat, req.query.sLon, req.query.eLon, req.query.del);
  res.send(result);
});
app.get('/renameRoute', async function (req, res) {
  if (!req.query.value.substring(req.query.value.length - 4) == ".gpx") {
    res.send("");
  }
  let test = GPXparser.renameRoute(path.join(__dirname + '/uploads/' + req.query.value), req.query.oName, req.query.nName);
  await update(req.query.nName,req.query.oName);
  res.send(test);
});

app.get('/renameTrack', function (req, res) {
  if (!req.query.value.substring(req.query.value.length - 4) == ".gpx") {
    res.send("");
  }
  let test = GPXparser.renameTrack(path.join(__dirname + '/uploads/' + req.query.value), req.query.oName, req.query.nName);
  res.send(test);
});
app.get('/showData', function (req, res) {
  if (!req.query.value.substring(req.query.value.length - 4) == ".gpx") {
    res.send("");
  }
  let test = GPXparser.ShowdataFromJSON(path.join(__dirname + '/uploads/' + req.query.value));
  res.send(test);
});
app.get('/showRouteData', function (req, res) {
  if (!req.query.value.substring(req.query.value.length - 4) == ".gpx") {
    res.send("");
  }
  let test = GPXparser.ShowRouteData(path.join(__dirname + '/uploads/' + req.query.value), req.query.RouteName);
  res.send(test);
});
app.get('/showTrackData', function (req, res) {
  if (!req.query.value.substring(req.query.value.length - 4) == ".gpx") {
    res.send("");
  }
  let test = GPXparser.ShowTrackData(path.join(__dirname + '/uploads/' + req.query.value), req.query.TrackName);
  res.send(test);
});

// SQL MANIPULATION
const mysql = require('mysql2/promise');

var h;
var u;
var p;
var d;

app.get('/logIn', async function (req, res) {
  let hostname = req.query.host;
  let username = req.query.user;
  let password = req.query.pass;
  let database = req.query.db;

  h = hostname;
  u = username;
  p = password;
  d = database;

  let dbConf = {
    host: hostname,
    user: username,
    password: password,
    database: database
  };
  let output = await connectDB(dbConf);
  console.log(output);
  res.send({ message: output });
});
async function connectDB(dbConf) {
  let results;
  let connection;
  try {
    connection = await mysql.createConnection(dbConf);
    await connection.execute("create table if not exists FILE (gpx_id INT NOT NULL AUTO_INCREMENT,  file_name VARCHAR(60) NOT NULL, ver DECIMAL(2,1) NOT NULL,creator VARCHAR(256) NOT NULL, primary key(gpx_id) )");
    await connection.execute("create table if not exists ROUTE (route_id INT NOT NULL AUTO_INCREMENT,  route_name VARCHAR(256),  route_len FLOAT(15,7) NOT NULL, gpx_id INT NOT NULL, primary key(route_id),FOREIGN KEY(gpx_id) REFERENCES FILE(gpx_id) ON DELETE CASCADE )");
    await connection.execute("create table if not exists POINT (point_id INT NOT NULL AUTO_INCREMENT,  point_index INT NOT NULL,  latitude DECIMAL(11,7) NOT NULL, longitude DECIMAL(11,7) NOT NULL,point_name VARCHAR(256),route_id INT NOT NULL, primary key(point_id),FOREIGN KEY(route_id) REFERENCES ROUTE(route_id) ON DELETE CASCADE  )");
    results = ("Connection Made");
  } catch (e) {
    console.log("Query error: " + e);
    results = ("Connection Not Made");
  } finally {
    if (connection && connection.end) connection.end();
  }
  console.log("inside function connectDB", results);
  return results;
}
app.get('/insertFileDB', async function (req, res) {
  let connection;
  let results;
  try {
    connection = await mysql.createConnection({
      host: h,
      user: u,
      password: p,
      database: d
    });
    await connection.execute("DELETE FROM FILE WHERE file_name = '" + req.query.f + "'");
    
    let insert = "INSERT INTO FILE (file_name, ver, creator) VALUES (" + "'" + req.query.f + "'," + req.query.v + "," + "'" + req.query.c + "')";
    await connection.execute(insert);
    results = ("Connection Made");

  } catch(e) {
    console.log("Query error: " + e);
    results = ("No Connection Made");
  } finally {
    if (connection && connection.end) connection.end();
  }
  res.send({ message: results });
});

app.get('/clearAllData', async function (req, res) {
  /* Create connection */
  let connection;

  let results;
  try {
    connection = await mysql.createConnection({
      host: h,
      user: u,
      password: p,
      database: d
    });
    await connection.execute("DELETE FROM FILE");
    results = ("Connection Made");
  } catch {
    results = ("No Connection Made");
  } finally {
    if (connection && connection.end) connection.end();
  }
  res.send({ message: results });
});
app.get('/displayDBStatus', async function (req, res) {
  let results;
  results = await display();
  console.log(results);
  res.send({ message: results });
});

async function display() {
  let results;
  let connection;
  try {
    connection = await mysql.createConnection({
      host: h,
      user: u,
      password: p,
      database: d
    });

    const [rows1, fields1] = await connection.execute('SELECT COUNT(*) AS elem FROM FILE');
    for (let row of rows1) {
      results = "Database has " + row.elem + " files";
    }
    const [rows2, fields2] = await connection.execute('SELECT COUNT(*) AS elem FROM ROUTE');
    for (let row of rows2) {
      results += ", " + row.elem + " routes, ";
    }
    const [rows3, fields3] = await connection.execute('SELECT COUNT(*) AS elem FROM POINT');
    for (let row of rows3) {
      results += row.elem + " points";
    }

  } catch (e) {
    console.log("Query error: " + e);
    results = ("Connection Not Made");

  } finally {
    if (connection && connection.end) connection.end();
  }
  return results;
}

app.get('/getFileID', async function (req, res) {
  let results = "OK";
  let connection;

  try {
    connection = await mysql.createConnection({
      host: h,
      user: u,
      password: p,
      database: d
    });
    
    let sqlQuery = "SELECT gpx_id FROM FILE WHERE (file_name = '" + req.query.value + "')";
    const [rows, fields] = await connection.execute(sqlQuery);

    for (let row of rows) {

      let id = row.gpx_id;
      let list = GPXparser.GPXViewtoJSON(path.join(__dirname + '/uploads/' + req.query.value));
      let data = JSON.parse(list);

      for (let routeDBIndex = 0; routeDBIndex < data.length; routeDBIndex++) {
        let string = data[routeDBIndex].component;
        let stripped = string.split(" ", 1);

        if (stripped == "Route") {
          let Rname = data[routeDBIndex].name;
          if(Rname == "") Rname = null;
          let Rlength = data[routeDBIndex].len;
          let insert = "REPLACE INTO ROUTE (route_name, route_len,gpx_id) VALUES (" + "'" + Rname + "'," +  Rlength + "," + id + ");";
          await connection.execute(insert);

          
          let sqlQuery1 = "SELECT route_id FROM ROUTE WHERE (route_name = '" + Rname + "' AND gpx_id = "+ id +")";
          const [rows2, fields2] = await connection.execute(sqlQuery1);
          let waypointsList;

          for (let row of rows2) {
            let id2 = row.route_id;
            waypointsList = GPXparser.waypointsFromJSON(path.join(__dirname + '/uploads/' + req.query.value), Rname);
            let data2 = JSON.parse(waypointsList);

            for (let j = 0; j < data2.length; j++) {
              let pointName = data2[j].name;
              if(pointName == "") pointName = null;
              let latitude = data2[j].latitude;
              let longtitude = data2[j].longtitude;
              let pointIndex = data2[j].pointIndex;
  
              let insert1 = "REPLACE INTO POINT (point_index,latitude,longitude,point_name,route_id) VALUES (" + pointIndex + ","+ latitude + ","+ longtitude + ","+"'" + pointName + "',"+ id2 + ");";
              // console.log(insert1);
              await connection.execute(insert1);
            }
          }

        }
      }
    }
  } catch (e) {
    console.log("Query error: " + e);
    results = ("No Connection Made");
  } finally {
    if (connection && connection.end) connection.end();
  }
  res.send({ message: results });
});
app.get('/executeQueries',async function(req,res){
	let results;
  let connection;
	try {
    connection = await mysql.createConnection({
      host: h,
      user: u,
      password: p,
      database: d
    });
    console.log(req.query.value);
    const [rows, fields] = await connection.execute(req.query.value);
    results = (rows);	
    console.log(rows);
  } catch (e) {
    console.log("Query error: " + e);
    results = { message: "No Connection Made" };
  }finally{
    if (connection && connection.end) connection.end();
  }
  res.send(results);
});
async function update(newName,oldName) {
  let connection;
  try {
    connection = await mysql.createConnection({
      host: h,
      user: u,
      password: p,
      database: d
    });

    let update = "UPDATE ROUTE SET route_name = '"+ newName +"'WHERE route_name = '"+oldName+"'";
    await connection.execute(update);
  } catch (e) {
    console.log("Query error: " + e);
  } finally {
    if (connection && connection.end) connection.end();
  }
}
async function updateAdd(routeName,fileName,filePath,latitude,longtitude,pointName) {
  let connection;
  try {
      connection = await mysql.createConnection({
        host: h,
        user: u,
        password: p,
        database: d
      });
     
      let len = GPXparser.routeLenFromJSON(routeName,filePath);
      let sqlQuery = "SELECT gpx_id FROM FILE WHERE (file_name = '" + fileName + "')";
      const [rows, fields] = await connection.execute(sqlQuery);
      for (let row of rows) {
        let id = row.gpx_id;
        if(routeName == "") routeName = null;
        let deletesql = "DELETE FROM ROUTE WHERE route_name = '" + routeName + "' AND gpx_id = '"+ id + "'";
        await connection.execute(deletesql);
        console.log(deletesql);
        
        let insert = "INSERT INTO ROUTE (route_name, route_len,gpx_id) VALUES (" + "'" + routeName + "'," +  len + "," + id + ")";
        await connection.execute(insert);
      }
      let sqlQuery1 = "SELECT route_id FROM ROUTE WHERE (route_name = '" + routeName + "')";
      const [rows2, fields2] = await connection.execute(sqlQuery1);
      
      for (let row of rows2) {
        let id2 = row.route_id;
        if(pointName == "") pointName = null;
        let deletesql = "DELETE FROM POINT WHERE route_id = '" + id2 + "'";
        await connection.execute(deletesql);
        console.log(rows2);
        let insert1 = "INSERT INTO POINT (point_index,latitude,longitude,point_name,route_id) VALUES (" + 0 + ","+ latitude + ","+ longtitude + ","+"'" + pointName + "',"+ id2 + ")";
        await connection.execute(insert1);
      }
  } catch (e) {
    console.log("Query error Add: " + e);
  } finally {
    if (connection && connection.end) connection.end();
  }
}
async function updateWpt(routeName,fileName,name) {
  let connection;
  try {
      connection = await mysql.createConnection({
        host: h,
        user: u,
        password: p,
        database: d
      });
      let len = GPXparser.routeLenFromJSON(routeName,fileName);
      let sqlQuery = "SELECT gpx_id FROM FILE WHERE (file_name = '" + name + "')";
      const [rows, fields] = await connection.execute(sqlQuery);
      for (let row of rows) {
        let id = row.gpx_id;
        if(routeName == "") routeName = null;
        let deletesql1 = "DELETE FROM ROUTE WHERE route_name = '" + routeName + "' AND gpx_id = '"+ id + "'";
        await connection.execute(deletesql1);
        console.log(deletesql1);
        
        let insert = "INSERT INTO ROUTE (route_name, route_len,gpx_id) VALUES (" + "'" + routeName + "'," +  len + "," + id + ")";
        await connection.execute(insert);
      }

      let waypointsList;
      let sqlQuery2 = "SELECT route_id FROM ROUTE WHERE (route_name = '" + routeName + "')";
      const [rows2, fields2] = await connection.execute(sqlQuery2);
      console.log(sqlQuery2);
      
      for (let row of rows2) {
        let id2 = row.route_id;
        let deletesql = "DELETE FROM POINT WHERE route_id = '" + id2 + "'";
        await connection.execute(deletesql);
        console.log(deletesql);

        waypointsList = GPXparser.waypointsFromJSON(fileName, routeName);
        let data2 = JSON.parse(waypointsList);
        
        for (let j = 0; j < data2.length; j++) {
          let pointName = data2[j].name;
          if(pointName == "") pointName = null;
          let latitude = data2[j].latitude;
          let longtitude = data2[j].longtitude;
          let pointIndex = data2[j].pointIndex;
          let insert1;
          console.log(data2);
          if(data2[0].pointIndex == 0) {
            insert1 = "INSERT INTO POINT (point_index,latitude,longitude,point_name,route_id) VALUES ("+(parseInt(pointIndex)).toString() + ","+ latitude + ","+ longtitude + ","+"'" + pointName + "',"+ id2 + ");";
          }else{
            insert1 = "INSERT INTO POINT (point_index,latitude,longitude,point_name,route_id) VALUES (" +(parseInt(pointIndex)+1).toString() + ","+ latitude + ","+ longtitude + ","+"'" + pointName + "',"+ id2 + ");";
          }
          await connection.execute(insert1);
        }
      }
  } catch (e) {
    console.log("Query error Wpt: " + e);
  } finally {
    if (connection && connection.end) connection.end();
  }
}

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);


