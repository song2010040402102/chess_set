function game()
{
	var m_ws;
	var m_mode = 0;
	var m_con_six = new connect_six();

	function adjust_pos_size()
	{
		var left = 0;
		var top = 0;
		var width = window.innerWidth;
		var height = window.innerHeight;		
		var offset = 25; //最好要大于滚动条的宽度，否则会出现排版错误，这是浏览器的bug
		var side = 0;		
		var ratio = 0.8;		
		var b_hor = 0;
		if(width > height)
		{
			if(width*ratio < height)
			{
				side = width*ratio - 2*offset;				
				top = (height - side)/2;	
				left = offset;			
			}
			else
			{
				side = height - 2*offset;
				left = (width*ratio - side)/2;
				top = offset;
			}		
			b_hor = 1;
		}
		else
		{
			if(height*ratio < width)
			{
				side = height*ratio - 2*offset;				
				left = (width - side)/2;	
				top = offset;			
			}
			else
			{
				side = width - 2*offset;
				top = (height*ratio - side)/2;
				left = offset;
			}				
		}		
		
		if(side < 60)
			return;

		var cv = document.getElementById("game_canvas");
		cv.style.marginLeft = left;
		cv.style.marginTop = top;		
		cv.style.marginRight = 0;
		cv.style.marginBottom = 0;
		cv.width = side;
		cv.height = side;

		var op = document.getElementById("game_operate");
		var form = document.getElementById("game_form");
		var me = document.getElementById("first_me");
		var you = document.getElementById("first_you");
		var text = document.getElementById("game_info");
		op.style.backgroundColor = "#ffffff";		
		if(b_hor)
		{
			op.style.marginTop = 0;			
			op.style.cssFloat = "right";
			op.style.textAlign = "center";			
			op.style.width = parseInt(width*(1-ratio)).toString() + "px";
			op.style.height = parseInt(height).toString() + "px";			
			
			form.style.cssFloat = "";
			form.style.width = op.style.width;
			form.style.height = parseInt(parseInt(op.style.height)*(1-ratio)).toString() + "px";
			form.style.paddingTop = parseInt(parseInt(form.style.height)*(1-ratio)/2).toString() + "px";
			form.style.paddingBottom = form.style.paddingTop;
			form.style.marginBottom = "0px";			

			me.style.padding = parseInt(parseInt(form.style.paddingTop)/2).toString() + "px";
			you.style.padding = me.style.padding;
			me.style.width = parseInt(parseInt(form.style.width)*ratio).toString() + "px";
			you.style.width = me.style.width;
			me.style.height = parseInt((parseInt(form.style.height)-parseInt(form.style.paddingTop)-4*parseInt(me.style.paddingTop))/2).toString() + "px";			
			you.style.height = me.style.height;			
			you.style.marginTop = form.style.paddingTop;
			me.style.fontSize = parseInt(parseInt(me.style.height)*ratio).toString() + "px";
			you.style.fontSize = me.style.fontSize;
					
			text.style.cssFloat = "";	
			text.style.marginRight = "";
			text.style.width = "95%";			
			text.style.height = parseInt(parseInt(op.style.height) - parseInt(form.style.height) - 2*parseInt(form.style.paddingTop) -offset) + "px";			
		}
		else
		{
			op.style.marginTop = height*ratio - top -side;		
			op.style.cssFloat = "";	
			op.style.textAlign = "";			
			op.style.width = parseInt(width).toString() + "px";
			op.style.height = parseInt(height*(1-ratio)).toString() + "px";			

			form.style.cssFloat = "left";
			form.style.width = parseInt(parseInt(op.style.width)*2*(1-ratio)).toString() + "px";				
			form.style.paddingTop = parseInt(parseInt(op.style.height)*(1-ratio)/2).toString() + "px";
			form.style.paddingBottom = form.style.paddingTop;
			form.style.height = parseInt(parseInt(op.style.height) - parseInt(form.style.paddingTop) - parseInt(form.style.paddingBottom)).toString() + "px";			
			form.style.marginRight = "0px";

			me.style.padding = parseInt(parseInt(form.style.paddingTop)/2).toString() + "px";
			you.style.padding = me.style.padding;
			me.style.width = parseInt(parseInt(form.style.width)*ratio).toString() + "px";
			you.style.width = me.style.width;
			me.style.height = parseInt((parseInt(form.style.height)-parseInt(form.style.paddingTop)-4*parseInt(me.style.paddingTop))/2).toString() + "px";			
			you.style.height = me.style.height;			
			you.style.marginTop = form.style.paddingTop;
			me.style.fontSize = parseInt(parseInt(me.style.height)*ratio).toString() + "px";
			you.style.fontSize = me.style.fontSize;

			text.style.cssFloat = "right";
			text.style.marginRight = parseInt(offset).toString() + "px";			
			text.style.height = "95%";
			text.style.marginTop = parseInt(parseInt(op.style.height)*0.025).toString() + "px";
			text.style.width = parseInt(parseInt(op.style.width) - parseInt(form.style.width) - offset) + "px";			
		}
	}

	this.init = function()
	{
		adjust_pos_size();
		m_con_six.init();
	}

	this.draw = function()
	{				
		var cv = document.getElementById("game_canvas");
		m_con_six.draw(cv);
	}

	this.size = function()
	{
		adjust_pos_size();
		this.draw();
	}

	this.click = function(pageX, pageY)
	{
		m_con_six.click(pageX, pageY);
	}

	this.button_click = function(id)
	{
		if(m_mode != 0)
			return;

		if(id == "first_me")
		{
			m_mode = 1;			
		}
		else if(id == "first_you")
		{
			m_mode = 2;			
		}

		m_ws = new WebSocket("ws://songxuxu.top:30999");
		m_ws.onopen = function()
		{
			console.log("connect success!");
			m_con_six.start_game(m_ws, m_mode);
		}        
		m_ws.onmessage = function (event) 
		{
			console.log("receive: " + event.data);
			m_con_six.on_message(event.data);
		}
		m_ws.onclose = function()
		{
			console.log("connect close!");
		}
	}
}