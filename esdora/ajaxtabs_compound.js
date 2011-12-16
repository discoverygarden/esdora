var pid;
var ajaxUrl;
$(document).ready(function(){
	  pid = $('#pid').val();
	  ajaxUrl=Drupal.settings.basePath;
	  ajaxUrl=ajaxUrl.concat("ajax/gettabs/compound");
	  //alert(ajaxUrl);
	  $('.Items').click(function(){
		//alert('click items');
		 $('#div1').css('display','block');
		 $('#viewers').css('display','none');			 
			
		 $('#div1').html('loading...');
		 $.ajax({
				type:'POST',
				url:ajaxUrl,
				data: {'pid' : pid,'tab':'Items'},
				success: function(result){
					 $('#div1').html(result);
					 $('.Metadata').css('background-color','#F4F4F4');
					 $('.Items').css('background-color','#D7D6DB');
					 $('.Description').css('background-color','#F4F4F4');
					 $('.History').css('background-color','#F4F4F4');
					 $('.Viewers').css('background-color','#F4F4F4');
					 Drupal.attachBehaviors('#div1');
					 return false;
				},
				dataType:"json"
			});
			return false;
			
		});
	  $('.Metadata').click(function(){
			//alert('click metadata');
		  	 $('#div1').css('display','block');
			 $('#viewers').css('display','none');		 
			
			 $('#div1').html('loading...');
			 $.ajax({
					type:'POST',
					url:ajaxUrl,
					data: {'pid' : pid,'tab':'Metadata'},
					success: function(result){
						 $('#div1').html(result);
						 $('.Metadata').css('background-color','#D7D6DB');
						 $('.Items').css('background-color','#F4F4F4');
						 $('.Description').css('background-color','#F4F4F4');
						 $('.History').css('background-color','#F4F4F4');
						 $('.Viewers').css('background-color','#F4F4F4');
						 Drupal.attachBehaviors('#div1');
					
					},
					dataType:"json"
				});
				
				
			});
	  $('.Description').click(function(){
		//alert('click Description');
		  $('#div1').css('display','block');
		 $('#viewers').css('display','none');		 
			
		  $('#div1').html('loading...');
		  var dWidth  = $(document).width()*.50;
		  var dHeight = $(document).height()*.75;
		  $.ajax({
				type:'POST',
				url:ajaxUrl,
				data: {'pid' : pid,'tab':'Description'},
				success: function(result){
					 $('#div1').html(result);
					 $('.Description').css('background-color','#D7D6DB');					 
					 $('.Metadata').css('background-color','#F4F4F4');
					 $('.Items').css('background-color','#F4F4F4');
					 $('.History').css('background-color','#F4F4F4');
					 $('.Viewers').css('background-color','#F4F4F4');
					 Drupal.attachBehaviors('#div1');
					 $('a.fb_dynamic').live('click',function(e){
						 e.preventDefault();
					       $(this).fancybox({  
								'width':dWidth,  
								'height':dHeight,  
								'autoScale'         : false,   
								'type'          : 'iframe'  
							});
					       
						}); 
					
				},
				dataType:"json"
			});
			
		});
	  $('.History').click(function(){
			//alert('click History');
		  	 $('#div1').css('display','block');
			 $('#viewers').css('display','none');			 
			
			 $('#div1').html('loading...');
			  var dWidth  = $(document).width()*.50;
			  var dHeight = $(document).height()*.75;
			 
			 $.ajax({
					type:'POST',
					url:ajaxUrl,
					data: {'pid' : pid,'tab':'History'},
					success: function(result){
						 $('#div1').html(result);
						 $('.History').css('background-color','#D7D6DB');							
						 $('.Metadata').css('background-color','#F4F4F4');
						 $('.Items').css('background-color','#F4F4F4');
						 $('.Description').css('background-color','#F4F4F4');
						 $('.Viewers').css('background-color','#F4F4F4');
						 Drupal.attachBehaviors('#div1');
						 $('a.fb_dynamic').live('click',function(e){
							 e.preventDefault();
						       $(this).fancybox({  
									'width':dWidth,  
									'height':dHeight,  
									'autoScale'         : false,   
									'type'          : 'iframe'  
								});
						       
							}); 
						
					},
					dataType:"json"
				});
				
			});
	  $('.Viewers').click(function(){
		 	 $('#div1').css('display','block');
			 $('#viewers').css('display','none');		 
			
			 $('#div1').html('loading...');
			 var dWidth  = $(document).width()*.70;
			  var dHeight = $(document).height()*.85;
			 $.ajax({
					type:'POST',
					url:ajaxUrl,
					data: {'pid' : pid,'tab':'Viewers'},
					success: function(result){
						 $('#div1').html(result);
						 $('.Viewers').css('background-color','#D7D6DB');
						 $('.Items').css('background-color','#F4F4F4');
						 $('.Description').css('background-color','#F4F4F4');
						 $('.History').css('background-color','#F4F4F4');
						 $('.Metadata').css('background-color','#F4F4F4');
						 Drupal.attachBehaviors('#div1');
						 $('a.fb_dynamic').live('click',function(e){
							 e.preventDefault();
						       $(this).fancybox({  
									'width':dWidth,  
									'height':dHeight,  
									'autoScale'         : false, 
									'transitionIn'       :   'elastic',
									'transitionOut'  :   'elastic',
									'type'          : 'iframe'  
								});
						       return false;
						       
							}); 
						},
					dataType:"json"
				});
							
		});	
	
	 
	 
})
