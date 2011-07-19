var pid;
var ajaxUrl;
$(document).ready(function(){
	  pid = $('#pid').val();
	  ajaxUrl=Drupal.settings.basePath;
	  ajaxUrl=ajaxUrl.concat("ajax/gettabs/atomic");
	  //alert(ajaxUrl);
	 $('.Items').css('background-color','#C0C0C0');		 
	  $('.Items').click(function(){
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
				},
				dataType:"json",
			});
			
		});
	  $('.Metadata').click(function(){
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
					dataType:"json",
				});
				
			});
	  $('.Description').click(function(){
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
					 $('.Viewers').css('background-color','#F4F4F4'); Drupal.attachBehaviors('#div1');
					 $('a.fb_dynamic').live('click',function(e){
						 e.preventDefault();
					       $(this).fancybox({  
								'width':dWidth,  
								'height':dHeight,  
								'autoScale'         : false,   
								'type'          : 'iframe'  
							}).trigger('click');
					       
						}); 
					
				},
				dataType:"json",
			});
			
		});
	  $('.History').click(function(){
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
					dataType:"json",
				});
				
			});
	  $('.Viewers').click(function(){
		 	 $('#div1').css('display','block');
			 $('#viewers').css('display','none');		 
			
			 $('#div1').html('loading...');
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
									'transitionIn' : 'none',
									'transitionOut' : 'none',
									'type'          : 'iframe',  
								});
						       
							}); 
						},
					dataType:"json",
				});
							
		});	
	 
	 
})