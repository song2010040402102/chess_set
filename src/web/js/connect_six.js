function connect_six()
{	
	var m_board;
	var m_board_size = 19;	
	var BLACK = 0;
	var WHITE = 1;
	var EMPTY = 2;
	var m_mode = 0;		
	var m_cur_step;	

	var m_ws;
	var m_cv;
	var BOARD_COLOR_S = "#F8963B";
	var BOARD_COLOR = parseInt(BOARD_COLOR_S.substring(1), 16);	

	//point struct
	function point()
	{
		this.x;
		this.y;
	}

	//step struct
	function step ()
	{
		this.side;
		this.first = new point();
		this.second = new point();
	}

	function init_board()
	{
		m_board = new Array();
		for(var i = 0; i < m_board_size; i++)
		{
			m_board[i] = new Array();
			for(var j = 0; j < m_board_size; j++)
			{
				m_board[i][j] = EMPTY;
			}
		}
	}

	function draw_chess(cv_black, cv_white, d)
	{
		d=parseInt(d);		
		if(d <= 0)
			return;

		cv_black.width = cv_black.height = d;
		cv_white.width = cv_white.height = d;
		var ctx_black = cv_black.getContext("2d");
		var ctx_white = cv_white.getContext("2d");
		var data_black = ctx_black.createImageData(cv_black.width, cv_black.height);
		var data_white = ctx_white.createImageData(cv_white.width, cv_white.height);

		var pixel=0.8, shadow=0.7;
		var Alias=true;

		var red=255-((BOARD_COLOR>>16)&255);
		var green=255-((BOARD_COLOR>>8)&255);
		var blue=255-((BOARD_COLOR>>0)&255);

		var pb,pw;
		var di,dj,d2=d/2.0-5e-1,r=d2-2e-1,f=Math.sqrt(3.0);
		var x,y,z,xr,xg,hh;
		var g;		

		for (var i=0; i<d; i++)
		{
			for (var j=0; j<d; j++)
			{	
				di=i-d2; dj=j-d2;
				hh=r-Math.sqrt(di*di+dj*dj);
				if (hh>=0)
				{	z=r*r-di*di-dj*dj;
				    if (z>0) z=Math.sqrt(z)*f;
				    else z=0;
					x=di; y=dj;
					xr=Math.sqrt(6*(x*x+y*y+z*z));
					xr=(2*z-x+y)/xr;
					if (xr>0.9) xg=(xr-0.9)*10;
					else xg=0;
					if (hh>pixel || !Alias)
					{
						g=10+10*xr+xg*140;
						pb=(g<<16)|(g<<8)|g;
						g=200+10*xr+xg*45;
						pw=(g<<16)|(g<<8)|g;
					}
					else
					{	hh=(pixel-hh)/pixel;
						g=10+10*xr+xg*140;
						var shade;
						if (di-dj<r/3) shade=1;
						else shade=shadow;

						pb= (((1-hh)*g+hh*shade*red)<<16)
							|(((1-hh)*g+hh*shade*green)<<8)
							|(((1-hh)*g+hh*shade*blue)<<0);

						g=200+10*xr+xg*45;

						pw= (((1-hh)*g+hh*shade*red)<<16)
							|(((1-hh)*g+hh*shade*green)<<8)
							|(((1-hh)*g+hh*shade*blue)<<0);
					}
				}
				else 
					pb=pw=BOARD_COLOR;															
				
			    var index = (j + i*d)*4;			    
			    data_black.data[index+0] = (pb>>16)&255;
			    data_black.data[index+1] = (pb>>8)&255;
			    data_black.data[index+2] = (pb>>0)&255;
			    data_black.data[index+3] = 255;			    

			    data_white.data[index+0] = (pw>>16)&255;
			    data_white.data[index+1] = (pw>>8)&255;
			    data_white.data[index+2] = (pw>>0)&255;
			    data_white.data[index+3] = 255;
			}
		}
		ctx_black.putImageData(data_black, 0, 0);
		ctx_white.putImageData(data_white, 0, 0);
	}

	function draw_board(cv)
	{				
		var side = cv.width;
		m_cv = cv;

		//填充棋盘色
		var ctx = cv.getContext("2d");
		ctx.fillStyle = BOARD_COLOR_S;
		ctx.fillRect(0, 0, cv.width, cv.height);											

		//画线
		var m_board_size = 19;
		for (var i = 1; i <= m_board_size; i++)
		{
			ctx.moveTo(side/(m_board_size+1), side*i/(m_board_size+1));
			ctx.lineTo(side*m_board_size/(m_board_size+1), side*i/(m_board_size+1));
		}	
		for (var i = 1; i <= m_board_size; i++)
		{
			ctx.moveTo(side*i/(m_board_size+1), side/(m_board_size+1));
			ctx.lineTo(side*i/(m_board_size+1), side*m_board_size/(m_board_size+1));
		}		
		ctx.stroke();

		//画字
		var fs = parseInt(side*2/3/(m_board_size+1));
		ctx.font = fs.toString() + "px 宋体";	
		ctx.fillStyle = "#000000";			
		for(var i = 0; i < m_board_size; i++)
		{			
			ctx.fillText(String.fromCharCode(65+i), side*5/6/(m_board_size+1) + side*i/(m_board_size+1), fs); //top
			ctx.fillText(String.fromCharCode(65+i), side/3/(m_board_size+1), side*6/5/(m_board_size+1) + side*i/(m_board_size+1)); //left
			ctx.fillText(i.toString(), side - side*2/3/(m_board_size+1), side*6/5/(m_board_size+1) + side*i/(m_board_size+1)); //right
			if(i<10)
			{
				ctx.fillText(i.toString(), side*5/6/(m_board_size+1) + side*i/(m_board_size+1), side-3); //bottom				
			}
			else
			{
				ctx.fillText(i.toString(), side*2/3/(m_board_size+1) + side*i/(m_board_size+1), side-3); //bottom				
			}
		}		

		//画棋子		
		var cv_black = document.createElement('canvas');
		var cv_white = document.createElement('canvas');
		draw_chess(cv_black, cv_white, side/(m_board_size+1));

		for(var i = 0; i < m_board_size; i++)
		{
			for(var j = 0; j < m_board_size; j++)
			{
				var x = side*i/(m_board_size+1)+side/2/(m_board_size+1);
				var y = side*j/(m_board_size+1)+side/2/(m_board_size+1);

				if(m_board[i][j] == BLACK)
					ctx.drawImage(cv_black, 0, 0, side/(m_board_size+1), side/(m_board_size+1), x, y, side/(m_board_size+1), side/(m_board_size+1));
				else if(m_board[i][j] == WHITE)
					ctx.drawImage(cv_white, 0, 0, side/(m_board_size+1), side/(m_board_size+1), x, y, side/(m_board_size+1), side/(m_board_size+1));
			}
		}
	}	

	function move_chess(x, y)
	{		
		if(m_cur_step.side == EMPTY)
		{						
			m_cur_step.side = BLACK;
			m_cur_step.first.x = m_cur_step.second.x = x;
			m_cur_step.first.y = m_cur_step.second.y = y;
		}
		else
		{
			if(m_cur_step.second.x == -1)
			{
				m_cur_step.second.x = x;
				m_cur_step.second.y = y;				
			}
			else
			{
				m_cur_step.side = m_cur_step.side^1;
				m_cur_step.first.x = x;
				m_cur_step.first.y = y;
				m_cur_step.second.x = -1;
				m_cur_step.second.y = -1;				
			}
		}	
		m_board[x][y] = m_cur_step.side;	

		if((m_mode == 1 || m_mode == 2) && m_cur_step.second.x != -1)
		{
			var msg = "type:consix;cmd:move;step:" + m_cur_step.side + "," +m_cur_step.first.x + "," + m_cur_step.first.y + "," + m_cur_step.second.x + "," + m_cur_step.second.y;
			m_ws.send(msg);
			console.log("send: " + msg);
		}
	}
	
	function parse_msg(msg)
	{
		var index1 = 0, index2 = 0;
		index1 = msg.indexOf("step:");

		if(index1 == -1)
			return false;
		
		index1 += String("step:").length;
		index2 = msg.indexOf(",", index1);
		m_cur_step.side = msg.substring(index1, index2);
		index1 = index2+1;
		index2 = msg.indexOf(",", index1);
		m_cur_step.first.x = msg.substring(index1, index2);
		index1 = index2+1;
		index2 = msg.indexOf(",", index1);
		m_cur_step.first.y = msg.substring(index1, index2);
		index1 = index2+1;
		index2 = msg.indexOf(",", index1);
		m_cur_step.second.x = msg.substring(index1, index2);
		index1 = index2+1;
		index2 = msg.length;
		m_cur_step.second.y = msg.substring(index1, index2);

		return true;
	}

	this.init = function()
	{
		init_board();

 		m_cur_step = new step();
		m_cur_step.side = EMPTY;
		m_cur_step.first.x = m_cur_step.first.y = -1;
		m_cur_step.second.x = m_cur_step.second.y = -1;
	}

	this.draw = function(cv)
	{		
		draw_board(cv);
	}

	this.click = function(pageX, pageY)
	{
		var offset_x = parseInt(m_cv.style.marginLeft);
		var offset_y = parseInt(m_cv.style.marginTop);
		var side = m_cv.width;		
		var x = parseInt((pageX-offset_x+side/2/(m_board_size+1))*(m_board_size+1)/side -1);
		var y = parseInt((pageY-offset_y+side/2/(m_board_size+1))*(m_board_size+1)/side -1);		
		console.log("pageX:"+pageX + "  pageY:"+pageY + "  point("+x+","+y+")");		
		if(x<0 || y<0 || x>m_board_size-1 || y>m_board_size-1)
			return;
		
		move_chess(x, y);

		draw_board(m_cv);		
	}

	this.start_game = function(ws, mode)
	{
		m_ws = ws;
		m_mode = mode;

		var msg = "type:consix;cmd:start;mode:" + m_mode;
		m_ws.send(msg);
		console.log("send: " + msg);
	}

	this.on_message = function(msg)
	{
		if(parse_msg(msg))
		{			
			m_board[m_cur_step.first.x][m_cur_step.first.y] = m_cur_step.side;
			if(m_cur_step.second.x == -1)
			{
				m_cur_step.second.x = m_cur_step.first.x;
				m_cur_step.second.y = m_cur_step.first.y;
			}							
			m_board[m_cur_step.second.x][m_cur_step.second.y] = m_cur_step.side;
			draw_board(m_cv);		
		}		
	}
}