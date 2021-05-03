// Put all onload AJAX calls here, and event listeners
$(document).ready(function() {
    document.getElementById('storeFilesDB').disabled = true; 
	document.getElementById('clearAllData').disabled = true; 
	document.getElementById('displayDBStatus').disabled = true; 
    document.getElementById('QueryList').disabled = true; 

    disableForm('RenameRoute',true);
    document.getElementById('renameButton').disabled = true;
    document.getElementById('filesList3').disabled = true;
    disableForm('AddWptForm',true);
    document.getElementById('AddWaypoint').disabled = true; 
    document.getElementById('filesList1').disabled = true;
    disableForm('AddRoute',true);
    document.getElementById('AddRouteButton').disabled = true;
    document.getElementById('filesList2').disabled = true; 

    // On page-load AJAX Example
    getFiles();
    $('#gpxViewPanel').change(function(e){
        e.preventDefault();
        let text =  getSelected('filesList0'); 
        if(text == "select a file"){ alert("please select a file");}
        else{updateView(text);}
    });
   
    $('#RouteButton').submit(function(e){
        let userFile =  getSelected('filesList1'); 
        let routeName = $('#nameRoute').val();
        let waypointName = $('#wptName2').val();
        let latitude = $('#lat2').val();
        let longtitude =$('#long2').val();
        let dataName =$('#dName').val();
        let dataValue =$('#dValue').val();
        e.preventDefault();
        if(userFile == "select a file"){
            alert("Error - please make sure that that you selected a file!");
        }else{
            if(routeName.length == 0) routeName = " ";
            if(dataValue.length != 0 && dataName.length == 0) alert("Error, You cannot have other data value without a name (Empty Name not allowed)");
            if(dataValue.length == 0 && dataName.length != 0) alert("Error, You cannot have other data name without a value (Empty value not allowed)");
            if(dataValue.length == 0 && dataName.length == 0 || dataValue.length != 0 && dataName.length != 0){
                if(longtitude <= 180 && longtitude >= -180 && latitude <= 90 && latitude >= -90){
                    $.ajax({
                        type: 'get',            //Request type
                        dataType: 'json',       //Data type - we will use JSON for almost everything 
                        async: true,
                        url: '/AddRoute',   //The server endpoint we are connecting to
                        data:{
                            value: userFile,
                            rte: routeName,
                            wpt: waypointName,
                            lat: latitude,
                            lon:longtitude,
                            dName: dataName,
                            dValue: dataValue
                        },
                        success: function () {
                          
                        },
                        fail: function(error) {
                            // Non-200 return, do something with error
                            console.log(error); 
                        }
                    });
                    alert("route has been added");
                    getFiles();
                    let viewPanelFile = getSelected('filesList0');
                    if(viewPanelFile == "select a file"){}
                    else{
                        if(viewPanelFile == userFile){
                            updateView(userFile);
                            // storeFilesDB(e);
                            
                        }
                    }
                    QueriesSelection2(e);
                }else{
                    alert("Error - Make sure that longtitude is more than -180 and less than or equal 180, while latitude is more than -90 and less than or equal 90 ");
                }
                
            }
            
           
        }

    });
    $('#ShowTrackData').submit(function(e){
        let userFile =  getSelected('filesList6'); 
        let trackName = $('#DataForm_Trackname').val();
        e.preventDefault();
        if(userFile == "select a file") alert("Error - please select a file");
        else{
            
            $.ajax({
                type: 'get',            //Request type
                dataType: 'json',       //Data type - we will use JSON for almost everything 
                url: '/showTrackData',   //The server endpoint we are connecting to
                data:{
                    value: userFile,
                    TrackName: trackName
                },
                success: function (data) {
                    
                    for(let i = 0; i < data.length; i++){
                        let string = "value:" + " " + data[i].name + " | "+ "Name: " + data[i].value +" ";
                        alert(string);
                    }
                },
                fail: function(error) {
                    console.log(error); 
                }
            });
        }
        
    });

    $('#ShowRouteData').submit(function(e){
        let userFile =  getSelected('filesList5'); 
        let routeName = $('#DataForm_Routename').val();
        e.preventDefault();
        if(userFile == "select a file") alert("Error - please select a file");
        else{

            $.ajax({
                type: 'get',            //Request type
                dataType: 'json',       //Data type - we will use JSON for almost everything 
                url: '/showRouteData',   //The server endpoint we are connecting to
                data:{
                    value: userFile,
                    RouteName: routeName
                },
                success: function (data) {
                    
                    for(let i = 0; i < data.length; i++){
                        let string = "value:" +" "+data[i].name + " | "+ "Name: " + data[i].value +" ";
                        alert(string);
                    }
                },
                fail: function(error) {
                    console.log("error"); 
                }
            });
           
        }
        
    });
    $('#renameTrack').submit(function(e){
        let userFile =  getSelected('filesList4'); 
        let originalT = $('#orginalTrackName').val();
        let newT = $('#newTrackName').val();
        e.preventDefault();
        if(userFile == "select a file") alert("Error - please select a file");
        else{
            $.ajax({
                type: 'get',            //Request type
                dataType: 'json',       //Data type - we will use JSON for almost everything 
                url: '/renameTrack',   //The server endpoint we are connecting to
                data:{
                    value: userFile,
                    oName: originalT,
                    nName: newT
                },
                success: function (data) {
                  
                    if(data.check == false){
                        alert("Track does not exist in file");
                    }else{
                        alert("track has been renamed");
                    }
                    let viewPanelFile = getSelected('filesList0');
                    if(viewPanelFile == "select a file"){}
                    else{
                        if(viewPanelFile == userFile){
                            updateView(userFile);
                        }
                    }

                },
                fail: function(error) {
                    console.log(error); 
                }
            });
        }

    });
    $('#RenameRoute').submit(function(e){
        let userFile =  getSelected('filesList3'); 
        let originalR = $('#originaRoute').val();
        let newR = $('#newRoute').val();
        e.preventDefault();
        if(userFile == "select a file") alert("Error - please select a file");
        else{

            $.ajax({
                type: 'get',            //Request type
                dataType: 'json',       //Data type - we will use JSON for almost everything 
                url: '/renameRoute',   //The server endpoint we are connecting to
                async: true,
                data:{
                    value: userFile,
                    oName: originalR,
                    nName: newR
                },
                success: function (data) { 
                    if(data.check == false){
                        alert("Route not exist in file");
                    }else{
                        alert("route has been renamed");
                    }
                    // QueriesSelection(e);
                    let viewPanelFile = getSelected('filesList0');
                    if(viewPanelFile == "select a file"){}
                    else{
                        if(viewPanelFile == userFile){
                            updateView(userFile);
                        }
                    }
                    QueriesSelection2(e);
                },
                fail: function(error) {
                    // Non-200 return, do something with error
                    console.log(error); 
                }
                
            });
        }
        
      
    });
    $('#waypointButton').click(function(e){
        let userFile =  getSelected('filesList2'); 
        let routeName = $('#routeNameWpt').val();
        let waypointName = $('#wptName').val();
        let latitude = $('#lat').val();
        let longtitude =$('#long').val();

        e.preventDefault();

        if(userFile == "select a file") alert("Error - please select a file");
        else{
            if(longtitude <= 180 && longtitude >= -180 && latitude <= 90 && latitude >= -90){
                
                $.ajax({
                    type: 'get',            //Request type
                    dataType: 'json',       //Data type - we will use JSON for almost everything 
                    url: '/AddWaypoint',   //The server endpoint we are connecting to
                    async: true,
                    data:{
                        value: userFile,
                        rte: routeName,
                        wpt: waypointName,
                        lat: latitude,
                        long: longtitude
                    },
                    success: function (data) {
                        if(data.check == false){
                            alert("route name does not exist");
                        }else {
                            alert("Waypoint has been Added");
                        }
                    },
                    fail: function(error) {
                        console.log(error); 
                    }
                });
                
                getFiles();
                let viewPanelFile = getSelected('filesList0');
                if(viewPanelFile == "select a file"){}
                else{
                    if(viewPanelFile == userFile){
                        updateView(userFile);
                    }
                }
                QueriesSelection2(e);
            }else{
                alert("Error - Make sure that longtitude is more than -180 and less than or equal 180, while latitude is more than -90 and less than or equal 90 ");
            }
        }

    });
    
    var addButtonFirst = 0;
    $('#AddWaypoint').click(function(e){
        
        if(addButtonFirst == 0){
            addChild();
        }
        if(addButtonFirst == 1){
            CloneForm();
        }
        addButtonFirst = 1;
        
        e.preventDefault();
    });

    $('#FindPath').submit(function(e){
        var check = 0;
        var table = document.getElementById("fileTable");
        let startLon = $('#Blong1').val();
        let endLon = $('#Blong2').val();
        let startLat = $('#Blat1').val();
        let endLat = $('#Blat2').val();
        let delta = $('#Bdelta').val();
        if((startLon > 180 || startLon < -180) || (startLat > 90 ||  startLat < -90)){
            alert("Error - Make sure that start longtitude is more than -180 and less than or equal 180, while start latitude is more than -90 and less than or equal 90 ");
        }
        if((endLon > 180 || endLon < -180) || (endLat > 90 || endLat < -90)){
            alert("Error - Make sure that end longtitude is more than -180 and less than or equal 180, while end latitude is more than -90 and less than or equal 90 ");
        }
        if($.isNumeric(startLat) == false || $.isNumeric(startLon) == false || $.isNumeric(endLat) == false || $.isNumeric(endLon) == false || $.isNumeric(delta) == false){
            alert("Error - make sure that start latitude, end latitude, start longtitude and end longtitude and delta are not empty and are numbers");
        }
        for (var i = 1, row; row = table.rows[i]; i++) {
            e.preventDefault();
            let userFile = row.cells[0].innerText;
            if(userFile == "select a file") alert("Error - please select a file");
            else{
                deleteAllRow('PathTable'); 
                addRow('PathTable',null,null,"-","-","-","-","-");  
                $.ajax({
                    type: 'get',            //Request type
                    dataType: 'json',       //Data type - we will use JSON for almost everything 
                    url: '/findPath',   //The server endpoint we are connecting to
                    data: {
                        value: userFile,
                        sLon: startLon,
                        eLon: endLon,
                        sLat: startLat,
                        eLat: endLat,
                        del: delta
                    },
                    success: function (data) {
                        if(check == 0){
                            deleteAllRow('PathTable');  
                            check = 1;
                        }
                        if(data.length!= 0){
                            for(let i = 0; i < data.length;i++) {
                                addRow('PathTable',null,null,data[i].component,data[i].name,data[i].numPoints,data[i].len,data[i].loop);     
                            }
                        }
                        e.preventDefault();

                    },
                    fail: function(error) {
                        // Non-200 return, do something with error
                        console.log(error); 
                    }
                });
                
            }
            e.preventDefault();
        }    

    });

    $('#createGPX').submit(function(e){
        let userFile = $('#fileToCreate').val().replace("C:\\fakepath\\", "");
        let version = $('#version').val();
        let creator = $('#creator').val();
        let numWaypoints = $('#waypointsNum').val();
        let numRoutes = $('#routesNum').val();
        let numTracks = $('#tracksNum').val();
        let namespace = $('#namespace').val();
        var alerted = 0;
        e.preventDefault();
        if(userFile.length == 0) {alert("Error - file cannot be left empty");
        }else{
                     
            if(Number.isInteger(parseInt(numWaypoints)) && Number.isInteger(parseInt(numRoutes)) && Number.isInteger(parseInt(numTracks)) && creator.length != 0) {
                if(namespace.length == 0) namespace = "http://www.topografix.com/GPX/1/1";
                if(version.length == 0) version = 1.1;   
                $.ajax({
                    type: 'get',            //Request type
                    dataType: 'json',       //Data type - we will use JSON for almost everything 
                    url: '/createGPXFile',   //The server endpoint we are connecting to
                    data: {
                        file: userFile,
                        ver: version,
                        cre: creator,
                        wpt: numWaypoints,
                        rte: numRoutes,
                        trk: numTracks,
                        nspace: namespace
                    },
                    success: function (data) {
                        alert("file has been added to file log pannel successfully");
                        data.forEach(element => {
                            let obj = JSON.parse(element);
                            if(userFile == obj.filename){
                                alert("Error - File already Exist");
                                alerted = 1;
                            }
                        }); 
        
                        if(alerted == 0){
                            addRow('fileTable',userFile,userFile,version,creator,numWaypoints,numRoutes,numTracks);               
                            for(let i = 0; i < 7; i++){
                                addtoList("filesList"+i,userFile);
                            }
                        }
                                
                        e.preventDefault();
        
                    },
                    fail: function(error) {
                        // Non-200 return, do something with error
                        console.log(error); 
                    }
                });
            }else{
                alert("Invalid Input has been Entered! Make sure creator field is not empty and that the fields for number of tracks, routes and waypoints are integers and not empty");
            }
        }

    });
    $('#logInButton').click(function(e){
        e.preventDefault();
        var n = $('#uname').val();
        var p = $('#password').val();
        var d = $('#dbname').val();
        var h = 'dursley.socs.uoguelph.ca';

        $.ajax({
            type: 'get',            	
            dataType: 'json', 
            async: true,
            url: '/logIn',
            data: {
                host: h,
                user: n,
                pass: p,
                db: d
            },   

            success: function (data) {
                console.log(data.message);
                if(data.message == 'Connection Not Made'){
                    alert("Connection Failure");
                    document.getElementById('storeFilesDB').disabled = true; 
                    document.getElementById('clearAllData').disabled =  true; 
                    document.getElementById('displayDBStatus').disabled =  true;
                    document.getElementById('QueryList').disabled = true; 
                }else{
                    msg = ("Connection Success");
                    alert(msg);
                    document.getElementById('storeFilesDB').disabled = false; 
                    document.getElementById('clearAllData').disabled = false; 
                    document.getElementById('displayDBStatus').disabled = false; 
                    document.getElementById('QueryList').disabled = false; 
                }
                e.preventDefault();
            },
            fail: function(error) {
                alert("fail"); 
            }
        });
    });
	$('#storeFilesDB').click(function(e){ 
        storeFilesDB(e);

        disableForm('RenameRoute',false);
        document.getElementById('renameButton').disabled = false;
        document.getElementById('filesList3').disabled = false;
        disableForm('AddWptForm',false);
        document.getElementById('AddWaypoint').disabled = false; 
        document.getElementById('filesList1').disabled = false;
        disableForm('AddRoute',false);
        document.getElementById('AddRouteButton').disabled = false;
        document.getElementById('filesList2').disabled = false; 
    });  

    $('#displayDBStatus').click(function(){
		displayDBStatus();
	});
	
    $('#clearAllData').click(function(){
		$.ajax({
			type: 'get',            
			dataType: 'json', 
            async: true,      
			url: '/clearAllData',   
			success: function (data) {
				alert("Table Cleared");
			},
			fail: function(error) {
				console.log(error); 
			}
		});
	});
    $('#ExecuteQuery').change(function(e){
        document.getElementById('QSubmit').style.display='block';  
        QueriesSelection(e);
    });
    $('#QSubmit').submit(function(e){
        QueriesSelection2(e);
    });
});
function QueriesSelection(e){
    e.preventDefault();
    let text =  getSelected('QueryList'); 
    if(text == "select a Query"){ alert("please select a Query");}
    else {
        document.getElementById('QueryTable').style.display='block';
        deleteAllRow('QueryTable');
        console.log("TEXT",text);
        if(text == 'Display all routes')  {
            DisplayQueryForms('QueryTwo','QueryThree','QueryFour','QueryFive','Q2','Q3','Q4','Q1','QueryOne');
        }
        if(text == 'Display the routes from a specific file'){
            DisplayQueryForms('QueryOne','QueryThree','QueryFour','QueryFive','Q1','Q3','Q4','Q2','QueryTwo');
        }
        if(text == 'Display all points of a specific route, ordered by point index') {
            DisplayQueryForms('QueryOne','QueryTwo','QueryFour','QueryFive','Q1','Q2','Q4','Q3','QueryThree');
        }
        if(text == 'Display all points from a specific file') {
            DisplayQueryForms('QueryOne','QueryTwo','QueryThree','QueryFive','Q1','Q2','Q3','Q4','QueryFour');
        }
        if(text == 'Display the N shortest/longest routes from a specific file') {
            DisplayQueryForms('QueryOne','QueryTwo','QueryThree','QueryFour','Q1','Q4','Q3','Q2','QueryFive');
        }

    }
}
function QueriesSelection2(e){
    e.preventDefault();
    let text =  getSelected('QueryList'); 
    if(text == "select a Query"){ alert("please select a Query");}
    else {
        document.getElementById('QueryTable').style.display='block';
        deleteAllRow('QueryTable');
        console.log("TEXT",text);
        if(text == 'Display all routes') QueryOne(e);
        if(text == 'Display the routes from a specific file') QueryTwo(e);
        if(text == 'Display all points of a specific route, ordered by point index') QueryThree(e);
        if(text == 'Display all points from a specific file') QueryFour(e);
        if(text == 'Display the N shortest/longest routes from a specific file') QueryFive(e);

    }
}


function storeFilesDB(e){
    var table = document.getElementById("fileTable");  
    for (var i = 1, row; row = table.rows[i]; i++) {
        e.preventDefault();
        let userFile = row.cells[0].innerText;  
        let objVer = row.cells[1].innerText;                
        let objCre = row.cells[2].innerText;  
           
        var file = userFile;
        var version = objVer;
        var cre = objCre;
       
        
        $.ajax({
            type: 'get',            	
            dataType: 'json', 
            async: true,  	
            url: '/insertFileDB',
            data: {
                f: file,
                v: version,
                c:cre
            },   
            success: function (data) {
                RoutesUpdateDB(userFile);
            },
            fail: function(error) {
                alert(error); 
            }
        });
     
    }
}
function QueryOne(e){
    console.log("text");
    e.preventDefault();
    console.log("here");
    let sqlQuery = "SELECT * FROM ROUTE ORDER BY ";
    let res = getSelected('Q1SortBy'); 
    if(res.localeCompare("select an option") == 0){
        alert("please select a valid option");
    }else{
        deleteAllRow('QueryTable');
        console.log("delete all rows");
        sqlQuery += res;
        console.log(res,sqlQuery);
        e.preventDefault();
        $.ajax({
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything 
            async:true,
            url: '/executeQueries',   //The server endpoint we are connecting to
            data: {
                value: sqlQuery
            },
            success: function (data) {  
                console.log("Q1",data);
                for(let i = 0; i < data.length;i++) {
                    addQuery('QueryTable',null,data[i].route_id,data[i].route_name,data[i].route_len,data[i].gpx_id,null,null);     
                }

            },
            fail: function(error) {
                // Non-200 return, do something with error
                console.log(error); 
            }
        });
    
    }  
}
function QueryTwo(e){
    e.preventDefault();
    let file = $("#Q2file").val();
    let sqlQuery = "SELECT * FROM ROUTE left join FILE on ROUTE.gpx_id = FILE.gpx_id WHERE FILE.file_name = '" + file + "' ORDER BY "
    let res = getSelected('Q2SortBy'); 
    if(res.localeCompare("select an option") == 0){
        alert("please select a valid option");
    }else{
        deleteAllRow('QueryTable2');
        sqlQuery += res;
        console.log(res);
        e.preventDefault();
        $.ajax({
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything 
            async:true,
            url: '/executeQueries',   //The server endpoint we are connecting to
            data: {
                value: sqlQuery
            },
            success: function (data) {  
                console.log(data);
                for(let i = 0; i < data.length;i++) {
                    addRow('QueryTable2',null,null,file,data[i].route_id,data[i].route_name,data[i].route_len,data[i].gpx_id);
                    
                }
            },
            fail: function(error) {
                // Non-200 return, do something with error
                console.log(error); 
            }
        });
    
    }  
}
function QueryThree(e){
    e.preventDefault(); 
    deleteAllRow('QueryTable3');
    let route = $("#Q3RouteName").val();
    let sqlQuery = "SELECT * FROM POINT left join ROUTE on POINT.route_id = ROUTE.route_id WHERE ROUTE.route_name = '" + route + "' ORDER BY point_index";
  
    e.preventDefault();
    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        async:true,
        url: '/executeQueries',   //The server endpoint we are connecting to
        data: {
            value: sqlQuery
        },
        success: function (data) {  
            console.log(data);
            for(let i = 0; i < data.length;i++) {
                console.log(route);
                if(data[i].point_name == "null") data[i].point_name = "unnamed point"+i;
                addQuery('QueryTable3',null,route,data[i].point_id,data[i].point_index,data[i].latitude,data[i].longitude,data[i].point_name,data[i].route_id);
                
            }
        },
        fail: function(error) {
            // Non-200 return, do something with error
            console.log(error); 
        }
    });
 
}
function QueryFour(e){
    e.preventDefault(); 
    
    let file = $("#Q4fileName").val();
    let sqlQuery = "SELECT * FROM ROUTE,POINT WHERE gpx_id =" +"(SELECT gpx_id FROM FILE WHERE file_name = " + JSON.stringify(file) +") " + "AND POINT.route_id = ROUTE.route_id ";

    let res = getSelected('Q4SortBy'); 
    if(res.localeCompare("select an option") == 0){
        alert("please select a valid option");
    }else{
        deleteAllRow('QueryTable4');
        sqlQuery +=" ORDER BY "+ res + ", point_index";
        console.log(sqlQuery);

        e.preventDefault();
        $.ajax({
            type: 'get', 
            dataType: 'json',       
            async:true,
            url: '/executeQueries', 
            data: {
                value: sqlQuery,
            },
            success: function (data) {  
                console.log("data2",data);
                for(let j = 0; j < data.length;j++) {
                    if(data[j].point_name == "null") data[j].point_name = "unnamed point"+j;
                    if(data[j].route_name == "null") data[j].route_name = "unnamed route"+j;
                    addQuery('QueryTable4',file,data[j].route_name,data[j].point_id,data[j].point_index,data[j].latitude,data[j].longitude,data[j].point_name,data[j].route_id);
                }
            },
            fail: function(error) {
                // Non-200 return, do something with error
                console.log(error); 
            }
        });
    }  
}
function QueryFive(e){
    e.preventDefault(); 
    
    let file = $("#Q5fileName").val();
    let N = $("#Q5NSize").val();
    let SOL =  getSelected('Q5SOL'); 
    console.log(file,N,SOL);
    
    let sqlQuery = "SELECT * FROM ROUTE left join FILE on ROUTE.gpx_id = FILE.gpx_id WHERE FILE.file_name = '" + file + "' ORDER BY "
    let res = getSelected('Q5SortBy'); 
    if(res.localeCompare("select an option") == 0){
        alert("please select a valid option");
    }else{
        deleteAllRow('QueryTable2');
        sqlQuery += res;
        if(SOL != "Shortest Routes" && SOL != "Longest Routes" ){ alert("please Select an option");}
        else {
            if(SOL == "Shortest Routes"){
                sqlQuery += " ASC";
            }else if(SOL == "Longest Routes"){
                sqlQuery += " DESC";
            }
            sqlQuery += " LIMIT " + N;
            console.log(res);
    
            e.preventDefault();
            $.ajax({
                type: 'get', 
                dataType: 'json',       
                async:true,
                url: '/executeQueries', 
                data: {
                    value: sqlQuery,
                },
                success: function (data) {  
                    console.log("data",data);
                    for(let i = 0; i < data.length;i++) {
                        addRow('QueryTable2',null,null,file,data[i].route_id,data[i].route_name,data[i].route_len,data[i].gpx_id);
                    }
                },
                fail: function(error) {
                    // Non-200 return, do something with error
                    console.log(error); 
                }
            });
        }
        
    }
  
}

function disableForm(id, status){
    var form = document.getElementById(id);
    var elements = form.elements;
    for (var i = 0, len = elements.length; i < len; ++i) {
        elements[i].readOnly = status;
    }
}

function DisplayQueryForms(idOne,idTwo,idThree,idFour,QUeryT,QUeryT2,QUeryT3,ChosenT,choosenID){
    document.getElementById(idOne).style.display='none';   //invisible
    document.getElementById(idTwo).style.display='none';  
    document.getElementById(idThree).style.display='none';  
    document.getElementById(idFour).style.display='none';   
    document.getElementById(QUeryT).style.display='none';  
    document.getElementById(QUeryT2).style.display='none';  
    document.getElementById(QUeryT3).style.display='none';  
    document.getElementById(ChosenT).style.display='block'; //visible
    document.getElementById(choosenID).style.display ='block';  
}

function RoutesUpdateDB(userFile){   
    console.log(userFile);
    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/getFileID',   //The server endpoint we are connecting to
        data: {
            value: userFile
        },
        success: function (data) {  
            console.log(data.message);                
        },
        fail: function(error) {
            // Non-200 return, do something with error
            console.log(error); 
        }
    });
} 

function updateView(userFile){
    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/updateGPX',   //The server endpoint we are connecting to
        data: {
            value: userFile
        },
        success: function (data) {
            deleteAllRow('dataTable');
            if(data.length == 0){
                addRow('dataTable',null,null,"-","-","-","-","-");    
            }
            for(let i = 0; i < data.length;i++) {
                addRow('dataTable',null,null, data[i].component,data[i].name,data[i].numPoints,data[i].len,data[i].loop);
            }
        },
        fail: function(error) {
            // Non-200 return, do something with error
            console.log(error); 
        }
    });
}
function displayDBStatus(){
    $.ajax({
        type: 'get',            	
        dataType: 'json',   
        async: true,	
        url: '/displayDBStatus',

        success: function (data) {
            console.log(data.message);
            alert(data.message);
        },
        fail: function(error) {
                alert(error); 
        }
    });
} 
function getFiles()
{
    
    let filename = $('#chosenFile').val().replace("C:\\fakepath\\", "");
    if(filename.substring(filename.length-4) != ".gpx" && filename.length != 0) alert("Please choose a valid gpx file");

    $.ajax({
        type: 'get',            //Request type
        url: '/getFileList',   //The server endpoint we are connecting to
        success: function (data) {
            
            deleteAllRow('fileTable');
            
            if(data.length == 0){
                addRow('fileTable',"No Files","Nofiles","-","-","-","-","-");    
            }
            for(let i = 0; i < 7; i++){

                removeFromList('#filesList'+i);
            }
          
            data.forEach(element => {
                let obj = JSON.parse(element);
                addRow('fileTable',obj.filename,obj.filename,obj.version,obj.creator,obj.numWaypoints,obj.numRoutes,obj.numTracks);
                for(let i = 0; i < 7; i++){
                    addtoList("filesList"+i,obj.filename);
                }
            });       
           
        },
        fail: function(error) {
            console.log(error); 
        }
    });
}

function findRow2RemoveIt(id, given_value) {
    var td = $("#" + id + " td");
    $.each(td, function(i) {
      if ($(td[i]).text() === given_value) {
        $(td[i]).parent().remove();
      } 
    });
}
function findRow(id, given_value) {
    var td = $("#" + id + " td");
    $.each(td, function(i) {
      if ($(td[i]).text() === given_value) {
        return true;
      } 
    });
    return false;
}
function addChild() {
    let form = document.getElementById('AddWptForm');
    form.style.display = 'block';
}
function CloneForm(idcollast) {
    var newel = $(idcollast).clone();
    newel.name += 1;
    newel.id += 1;
    $(newel).insertAfter(idcollast);
}

function addtoList(id,filename){
    let select = document.getElementById(id);
    let option = document.createElement("option");
    option.text = filename;
    option.value = filename;
    select.appendChild(option);
}

function getSelected(id)
{
   let select = document.getElementById(id);
   return select.options[select.selectedIndex].text;
}
function removeFromList(id){
    $(id).find('option').remove().end().append('<option value="select a file">select a file</option>').val();
}   
function addRow(id,filename,address,creator,version,wpt,rte,trk) {
    let table = document.getElementById(id);
    let e = table.rows.length-1;
    let x = table.insertRow(e+1);
    let l = table.rows[e].cells.length;

    for (let c=0, m=l; c < m; c++) {
        table.rows[e+1].insertCell(c);
    }
    if(filename != null && address != null){
        table.rows[e+1].cells[0].innerHTML = '<a href="' + address + '"> '  + filename + '</a>';
        table.rows[e+1].cells[1].innerHTML = creator;
        table.rows[e+1].cells[2].innerHTML = version;
        table.rows[e+1].cells[3].innerHTML = wpt;
        table.rows[e+1].cells[4].innerHTML = rte;
        table.rows[e+1].cells[5].innerHTML = trk;
    }else if(filename == null && address == null && rte != null && trk != null) {
        table.rows[e+1].cells[0].innerHTML = creator;
        table.rows[e+1].cells[1].innerHTML = version;
        table.rows[e+1].cells[2].innerHTML = wpt;
        table.rows[e+1].cells[3].innerHTML = rte;
        table.rows[e+1].cells[4].innerHTML = trk;
    }else{
        table.rows[e+1].cells[0].innerHTML = creator;
        table.rows[e+1].cells[1].innerHTML = version;
    }
    
}
function addQuery(id,refFile,refName,table_id,table_name,table_len,ref_id,latitude,longtitude) {
    let table = document.getElementById(id);
    let e = table.rows.length-1;
    let x = table.insertRow(e+1);
    let l = table.rows[e].cells.length;

    for (let c=0, m=l; c < m; c++) {
        table.rows[e+1].insertCell(c);
    }
    if(latitude == null && longtitude == null && refFile == null){
        table.rows[e+1].cells[0].innerHTML = refName;
        table.rows[e+1].cells[1].innerHTML = table_id;
        table.rows[e+1].cells[2].innerHTML = table_name;
        table.rows[e+1].cells[3].innerHTML = table_len;
    }else if(latitude != null && longtitude != null && refFile == null ){
        table.rows[e+1].cells[0].innerHTML = refName;
        table.rows[e+1].cells[1].innerHTML = table_id;
        table.rows[e+1].cells[2].innerHTML = table_name;
        table.rows[e+1].cells[3].innerHTML = table_len;
        table.rows[e+1].cells[4].innerHTML = ref_id;
        table.rows[e+1].cells[5].innerHTML = latitude;
        table.rows[e+1].cells[6].innerHTML = longtitude;
    }
    else{
        table.rows[e+1].cells[0].innerHTML = refFile;
        table.rows[e+1].cells[1].innerHTML = refName;
        table.rows[e+1].cells[2].innerHTML = table_id;
        table.rows[e+1].cells[3].innerHTML = table_name;
        table.rows[e+1].cells[4].innerHTML = table_len;
        table.rows[e+1].cells[5].innerHTML = ref_id;
        table.rows[e+1].cells[6].innerHTML = latitude;
        table.rows[e+1].cells[7].innerHTML = longtitude;
    }
    
}
function addHeader(id,filename,creator,version,wpt,rte,trk) {
    let table = document.getElementById(id);
    let e = table.rows.length-1;
    let x = table.insertRow(e+1);
    let l = table.rows[e].cells.length;

    for (let c=0, m=l; c < m; c++) {
        table.rows[e+1].insertCell(c);
    }
    if(filename != null && address != null){
        table.rows[e+1].cells[0].innerHTML = filename
        table.rows[e+1].cells[1].innerHTML = creator;
        table.rows[e+1].cells[2].innerHTML = version;
        table.rows[e+1].cells[3].innerHTML = wpt;
        table.rows[e+1].cells[4].innerHTML = rte;
        table.rows[e+1].cells[5].innerHTML = trk;
    }
}

function addColumn(id,text) {
    let table = document.getElementById(id);
    let e = table.rows.length-1;
    let l = table.rows[e].cells.length;
    for (let r = 0, n = table.rows.length; r < n; r++) {
        table.rows[r].insertCell(l);
        table.rows[r].cells[l].innerHTML = text.bold();
    }
}

function deleteRow(id) {
    let table = document.getElementById(id);
    let e = table.rows.length-1;
    table.deleteRow(e);
}

function deleteColumn(id) {
    let table = document.getElementById(id);
    let e = table.rows.length-1;
    let l = table.rows[e].cells.length;
    for (let r = 0, n = table.rows.length; r < n; r++) {
        table.rows[r].deleteCell(l);
    }
}
function deleteAllRow(id) {
    let table = document.getElementById(id);
    let e = table.rows.length-1;
    for(let i = 0; i < e; i++){
        table.deleteRow(1);
    }
}

// SQL MANIPULATION 
