import java.io.*;
import java.net.*;
import java.util.Scanner;

class KVS_Client {

  public static int port = 33322;
  public static String hostname;

  public static class InputHandler extends Thread {
    private BufferedReader in;
    public InputHandler(BufferedReader in) {
      this.in = in;
    }

    public void run() {
      try {
        String line;
        while ((line = in.readLine()) != null)
          System.out.println(line);
      } catch (IOException e) {
        System.err.println("Connection to server lost");
      }
    }
  }


  public static void main(String[] args) throws IOException {
    hostname = args[0];

    Socket sock;
    try {
      sock = new Socket(hostname, port);

      BufferedReader in = new BufferedReader(new InputStreamReader(sock.getInputStream()));
      PrintWriter out = new PrintWriter(sock.getOutputStream(), true);
      Scanner stdin = new Scanner(System.in);

      InputHandler inputThread = new InputHandler(in);
      inputThread.start();

      String input;
      boolean active = true;
      while (active) {
        input = stdin.nextLine();
        out.println(input);
        if (input.equals("exit"))
          active = false;
      }

      inputThread.join();

      in.close();
      out.close();
      sock.close();

    } catch (UnknownHostException e) {
      System.err.println("Cannot identify server");
    } catch (IOException e) {
      System.err.println("Communication fail");
    } catch (InterruptedException e) {
      System.err.println("Thread fail");
    }
  }

}
