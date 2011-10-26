
//var ajaxUrl;
//var datefrom;
//var dateto;
//var dates;
$(document).ready(function(){
	 dates = $( "#from, #to" ).datepicker({
			numberOfMonths: 1,
                        minDate: new Date(1900,1-1,1),
                        changeMonth:true,
                        changeYear:true,
                        dateFormat:'yy-mm-dd',
			onSelect: function( selectedDate ) {
				var option = this.id == "from" ? "minDate" : "maxDate",
					instance = $( this ).data( "datepicker" ),
					date = $.datepicker.parseDate(
						instance.settings.dateFormat ||
						$.datepicker._defaults.dateFormat,
						selectedDate, instance.settings );
				dates.not( this ).datepicker( "option", option, date );
                        },
                        onClose:function(){
                            datefrom=$("#from").val();
                            dateto=$("#to").val();
                            ajaxUrl=Drupal.settings.basePath;
                            ajaxUrl=ajaxUrl.concat("mapsearch/dates");       
                            $.ajax({
                                        type:'POST',
                                        url:ajaxUrl,
                                        data: {"datefrom":datefrom, "dateto": dateto},
                                        success: function(result){
                                                $('#date').html(result);   
                                    },
                                    dataType:"json"
                             });        
			}
                        
		});
       
        init();     
        
        $("#refresh").click(function(){
		location.reload(); 
	});
        
})

 function init()
 {
     
     //$.datepicker.setDefaults({ dateFormat: 'yy-mm-dd' });

	ajaxUrl=Drupal.settings.basePath;
	ajaxUrl=ajaxUrl.concat("mapsearch/mapCoor");
	//document.namespaces;
	var map = new OpenLayers.Map('map');
        var layer = new OpenLayers.Layer.WMS( "OpenLayers WMS",
                "http://vmap0.tiles.osgeo.org/wms/vmap0", {layers: 'basic'} );
        
        var control = new OpenLayers.Control();
        
       
        OpenLayers.Util.extend(control, {
	            draw: function () {
	                // this Handler.Box will intercept the shift-mousedown
	                // before Control.MouseDefault gets to see it
	                this.box = new OpenLayers.Handler.Box( control,
	                    {"done": this.notice},
	                    {keyMask: OpenLayers.Handler.click});
	                this.box.activate();
	            },
                    
	            notice: function (bounds) {
	                var ll = map.getLonLatFromPixel(new OpenLayers.Pixel(bounds.left, bounds.bottom)); 
	                var ur = map.getLonLatFromPixel(new OpenLayers.Pixel(bounds.right, bounds.top));
	                box = ll.lon.toFixed(4) + ", " + 
	                		  ll.lat.toFixed(4) + ", " + 
	                		  ur.lon.toFixed(4) + ", " + 
	                		  ur.lat.toFixed(4);
	               //alert(box);
                      $.ajax({
	                  	type:'POST',
	                  	url:ajaxUrl,
	                  	data: {"box": box},
	                  	success: function(result){
	                  		$('#coordinates').html(result);
                               },
	                  	dataType:"json"
	                  });
	               
	            }
        });
        
        

        map.addLayer(layer);
        map.addControl(control);
        map.setCenter(new OpenLayers.LonLat(-85, 0), 2, false);
        map.zoomOut();
        //map.zoomToMaxExtent();
    
}
