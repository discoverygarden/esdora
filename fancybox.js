/*fancybox.js*/
$(document).ready(function(){  

	$('a.fb_dynamic').each(function(){  
		var dWidth  = $(document).width()*.50;
	    var dHeight = $(document).height()*.50;     
		$(this).fancybox({  
			'width':dWidth,  
			'height':dHeight,  
			'autoScale'         : false,  
			'transitionIn'      : 'elastic',  
			'transitionOut'     : 'elastic',  
			'type'          : 'iframe'  
		});  
	}); 
});