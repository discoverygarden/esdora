
var ajaxUrl;
$(document).ready(function(){
	init();
	$("#refresh").click(function(){
		location.reload(); 
		
		
	});
        
})

 function init()
 {
	ajaxUrl=Drupal.settings.basePath;
	ajaxUrl=ajaxUrl.concat("mapsearch/test");
	document.namespaces;
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
        map.setCenter(new OpenLayers.LonLat(0, 0), 3);
}
