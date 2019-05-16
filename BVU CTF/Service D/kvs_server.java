//kvs_server
import java.io.*;
import java.net.*;
import java.util.concurrent.ConcurrentHashMap;
import java.nio.file.Files;
import java.nio.file.Paths;


class KVS_Server {

  public static int port = 33322;
  public static ConcurrentHashMap<String, Value> store = new ConcurrentHashMap<String, Value>();

  public static String getFileContents(String fileName) {
    String contents;
    try {
      contents = new String(Files.readAllBytes(Paths.get(fileName)));
      if (contents.charAt(contents.length()-1) == '\n')
        contents = contents.substring(0, contents.length()-1);
    } catch (IOException e) {
      contents = "Failed to read file";
    }
    return contents;
  }

  public static class Value {
    public String val;
    public boolean locked;

    public Value(String val) {
      this.val = val;
      this.locked = false;
    }

    public Value(String val, boolean locked) {
      this.val = val;
      this.locked = locked;
    }
  }

public static class KeyValStore extends Thread {
    private Socket clientSocket;
    private BufferedReader in;
    private PrintWriter out;
    private String key;
    private String val;
    private String input;

    public KeyValStore(Socket clientSocket) {
      this.clientSocket = clientSocket;
    }

    public void run() {
      //System.out.println("Client Connected");

      try {
        in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
        out = new PrintWriter(clientSocket.getOutputStream(), true);

        out.println(getFileContents(store.get("welcome").val));


        boolean active = true;
        while (active) {
          out.println(getFileContents(store.get("menu").val));

          input = in.readLine();

          if (input.equals("add")) {
            add();
          } else if (input.equals("update")) {
            update();
          } else if (input.equals("get")) {
            get();
          } else if (input.equals("delete")) {
            delete();
          } else if (input.equals("about")) {
            out.println(getFileContents(store.get("about").val));
          } else if (input.equals("exit")) {
            out.println(getFileContents(store.get("goodbye").val));
            active = false;
          } else {
            out.println("\nInvalid selection, try again.");
          }
        }

        in.close();
        out.close();
        clientSocket.close();
        //System.out.println("Client disconnected");
      } catch (Exception e) {
        //System.out.println("Client quit unexpectedly");
      }
    }


    private String prompt(String str) throws IOException {
      out.println(str);
      return in.readLine();
    }

    private void add() {
      try {
        key = prompt("Enter Key:");
        val = prompt("Enter Val:");
      } catch (Exception e) {
        System.err.println("I/O issue: " + e.toString());
      }

      if (store.containsKey(key))
        out.println("Error: Key already exists in the store");
      else
        store.put(key, new Value(val));
    }

    private void update() {
      try {
        key = prompt("Enter Key:");
        val = prompt("Enter Val:");
      } catch (Exception e) {
        System.err.println("I/O issue: " + e.toString());
      }

      if (store.containsKey(key)) {
        boolean locked = false;
        if (store.get(key).locked) {
          try {
            String userPass = prompt("Key is locked, enter password:");
            BufferedReader br = new BufferedReader(new FileReader("password.txt"));
            String realPass = br.readLine();
            br.close();
            if (!userPass.equals(realPass))
              locked = true;
          } catch (Exception e) {
            System.err.println("I/O issue: " + e.toString());
          }
        }

        if (!locked)
          store.replace(key, new Value(val, store.get(key).locked));
        else
          out.println("Error: You are not authorized to update that key");
      } else
        out.println("Error: Key does not exist in the store");
    }
    private void get() {
      try {
        key = prompt("Enter Key:");
      } catch (Exception e) {
        System.err.println("I/O issue: " + e.toString());
      }

      val = (store.containsKey(key)) ? store.get(key).val :
        "Error: Key does not exist in the store";
      out.println(val);
    }

    private void delete() {
      try {
        key = prompt("Enter Key:");
      } catch (Exception e) {
        System.err.println("I/O issue: " + e.toString());
      }

      if (store.containsKey(key)) {
        store.remove(key);
        out.println("Key removed");
      } else
        out.println("Error: Key does not exist in the store");
    }
  }
 public static void main(String[] args) {
    System.out.println("[KeyValStore Server]");

    store.put("welcome", new Value("welcome.txt", true));
    store.put("menu", new Value("menu.txt", true));
    store.put("goodbye", new Value("goodbye_1.txt", true));
    store.put("about", new Value("about.txt", true));

    ServerSocket serverSocket;
    try {
      serverSocket = new ServerSocket(port);
    } catch(IOException e) {
      System.err.println("Failed to bind server to port(" + port + ")");
      return;
    }

    while (true) {
      try {
        Socket clientSocket = serverSocket.accept();
        (new KeyValStore(clientSocket)).start();
      } catch (Exception e) {
        System.err.println("Something bad happened when setting up a new client connection");
      }
    }
  }
}
