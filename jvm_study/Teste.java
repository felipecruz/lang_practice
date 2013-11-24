import java.util.Map;
import java.util.List;
import java.util.ArrayList;

public class Teste {
    private int c;

    public Teste(int c, double d) {
        this.c = c;
    }

    public static void request(Map request) {
        int i = 0;
        if (request.get("uri") != null) {
            i += 1;
        }
    }

    public static void main(String argv[]) throws Exception {
        List<Teste> list = new ArrayList<Teste>();

        Teste t = new Teste(0, 0);
        for(int i = 0; i < 1000000; i++) {
            t = new Teste(i, (double) i);
            list.add(t);
        }

    }
}
