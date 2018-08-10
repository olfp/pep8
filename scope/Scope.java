import java.io.*;
import java.net.*;
import java.awt.Color;
import java.awt.Point;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.event.*;
import java.util.*;


import javax.swing.JFrame;
import javax.swing.JPanel;
 
public class Scope extends JPanel implements Runnable {
	
  private Thread runner;
  protected static List<TimePoint> displaylist = new ArrayList<TimePoint>();
  private Color bgColor = new Color( 0x50, 0x50, 0x50 );
  private Color fgColor = new Color( 0xfe , 0xb3 , 0x08 );
  private static Boolean init = true;
  
  public Scope() {
	  
	runner = new Thread(this);
	runner.start();
  }

  @Override
  public void paintComponent(Graphics g) {
    super.paintComponent(g);

    Graphics2D g2d = (Graphics2D) g;

	/*
    g2d.setColor(Color.red);

    for (int i = 0; i <= 100; i++) {
      Dimension size = getSize();
      int w = size.width ;
      int h = size.height;

      Random r = new Random();
      int x = Math.abs(r.nextInt()) % w;
      int y = Math.abs(r.nextInt()) % h;
      g2d.drawLine(x, y, x, y);
    }
	*/
	
	Dimension d = getSize();
	g2d.setBackground(bgColor);
	g2d.clearRect(0,0,d.width,d.height);
	
	for(TimePoint p : displaylist) {
		g2d.setColor(fgColor);
		g2d.drawLine(p.x, p.y, p.x, p.y);
	}

  }
  
  public void run() {
	  Thread thisThread = Thread.currentThread();
	  while (runner == thisThread) {
		repaint();	
        try {
            Thread.sleep(10);
       } catch (InterruptedException e) { }		  
	  }
	  
  }
  
  public static void main(String[] args) {
	int port = (args.length < 1) ? 4321 : Integer.parseInt(args[0]);

    Scope points = new Scope();
    JFrame frame = new JFrame("Scope");
    frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    frame.add(points);
    frame.setSize(512, 512);
    frame.setLocationRelativeTo(null);
    frame.setVisible(true);

	
    try (ServerSocket serverSocket = new ServerSocket(port)) {
 
        System.out.println("Server is listening on port " + port);
	 	init = false;
  
        while (true) {
            Socket socket = serverSocket.accept();
            System.out.println("New client connected");
			
			InputStream input = socket.getInputStream();
            BufferedReader reader = new BufferedReader(new InputStreamReader(input));
 
            OutputStream output = socket.getOutputStream();
            PrintWriter writer = new PrintWriter(output, true);
 
 
            String text;
 
            do {
            	writer.print("Scope> ");
				writer.flush();
                text = reader.readLine();
				String[] coord = text.split(",");
				int x = Integer.parseInt(coord[0]);
				int y = Integer.parseInt(coord[1]);
				displaylist.add(new TimePoint(x,y));
            } while (!text.equals("bye"));
			
            socket.close();
        }
 
    } catch (IOException ex) {
        System.out.println("Server exception: " + ex.getMessage());
        ex.printStackTrace();
    }
  }
}