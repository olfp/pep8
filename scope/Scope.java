import java.io.*;
import java.net.*;
import java.awt.Color;
import java.awt.Point;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.EventQueue;
import java.awt.event.*;
import java.util.*;


import javax.swing.JFrame;
import javax.swing.JPanel;
 
public class Scope extends JPanel implements Runnable {
	
    private Thread runner;
    protected static List<TimePoint> displaylist = new LinkedList<TimePoint>();
    //private Color bgColor = new Color( 0x50, 0x50, 0x50 );
    //private Color fgColor = new Color( 0xfe , 0xb3 , 0x08 );
    private Color bgColor = new Color( 0x0, 0x0, 0x0 );
    private float fgHue = .42F; //0.42F / 360.F;
    private float fgSat = 1.F; // 0.97F; 
    private static int phosfade = 500;
  
    public Scope() {
	  
	runner = new Thread(this);
	runner.start();
    }

    @Override
    public void paintComponent(Graphics g) {
	super.paintComponent(g);

	Graphics2D g2d = (Graphics2D) g;
	
	Dimension d = getSize();
	//g2d.setColor(fgColor);
	g2d.setBackground(bgColor);
	g2d.clearRect(0,0,d.width,d.height);

	while(displaylist.size() > 0 && displaylist.get(0).t <= 0) {
	    displaylist.remove(0);
	}
	for(int idx = 0; idx < displaylist.size(); idx++) {
	    TimePoint p = displaylist.get(idx); 
	    g2d.setColor(Color.getHSBColor(fgHue, fgSat, ((float)p.t / (float)phosfade)));
	    g2d.drawLine(p.x, p.y, p.x, p.y);
	    p.t--;
	}

    }
  
    @Override
    public void run() {
	Thread thisThread = Thread.currentThread();
	while (runner == thisThread) {
	    repaint();	
	    try {
		Thread.sleep(1);
	    } catch (InterruptedException e) { }		  
	}
	  
    }
  
    public static void main(String[] args) {
	int port = (args.length < 1) ? 4321 : Integer.parseInt(args[0]);

	EventQueue.invokeLater(new Runnable() {

		@Override
		public void run() {	
		    Scope scope = new Scope();
		    JFrame frame = new JFrame("Scope");
		    frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		    frame.add(scope);
		    scope.setPreferredSize(new Dimension (512, 512));
		    frame.pack();
		    frame.setLocationRelativeTo(null);
		    frame.setVisible(true);
		}
	    });

	
	try (ServerSocket serverSocket = new ServerSocket(port)) {
 
	    System.out.println("Server is listening on port " + port);
  
	    while (true) {
		Socket socket = serverSocket.accept();
		System.out.println("New client connected");
			
		InputStream input = socket.getInputStream();
		BufferedReader reader = new BufferedReader(new InputStreamReader(input));
 
		OutputStream output = socket.getOutputStream();
		PrintWriter writer = new PrintWriter(output, true);
 
 
		String text;
 
		while((text = reader.readLine()) != null) {
		    //System.out.println("Q: " + displaylist.size() + "; COORDS: " + text);
		    if(text != null) {
			String[] coord = text.split(",");
			int x = Integer.parseInt(coord[0]);
			int y = Integer.parseInt(coord[1]);
			TimePoint p = new TimePoint(x,y, phosfade);
			if(!displaylist.contains(p)) {
			    displaylist.add(p);
			} else {
			    TimePoint o = displaylist.get(displaylist.indexOf(p));
			    o.t = phosfade;
			}
		    }
		}
			
		socket.close();
	    }
 
	} catch (IOException ex) {
	    System.out.println("Server exception: " + ex.getMessage());
	    ex.printStackTrace();
	}
    }
}
