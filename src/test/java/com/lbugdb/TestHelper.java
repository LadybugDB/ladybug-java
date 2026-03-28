package com.ladybugdb;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class TestHelper {
    private static Database db;
    private static Connection conn;
    private static final Path projectDir = Paths.get(System.getProperty("ladybug.projectDir", "."));
    private static final Path tinysnbDir = projectDir.resolve("../../dataset/tinysnb").normalize();
    private static final Path tinysnbSerialDir = projectDir.resolve("../../dataset/tinysnb-serial").normalize();
    private final static String extensions = "csv|parquet|npy|ttl|nq|json|lbug_extension";
    private final static Pattern dataFileRegex = Pattern.compile("\"([^\"]+\\.(" + extensions + "))\"", Pattern.CASE_INSENSITIVE);

    public static Database getDatabase() {
        return db;
    }

    public static Connection getConnection() {
        return conn;
    }

    public static void loadData(String dbPath) throws IOException {
        BufferedReader reader;
        db = new Database(dbPath);
        conn = new Connection(db);

        String line;

        reader = new BufferedReader(new FileReader(tinysnbDir.resolve("schema.cypher").toFile()));
        while ((line = reader.readLine()) != null) {
            line = line.trim();
            if (line.isEmpty()) {
                continue;
            }
            try (QueryResult result = conn.query(line)) {
            }
        }
        reader.close();


        reader = new BufferedReader(new FileReader(tinysnbDir.resolve("copy.cypher").toFile()));
        while ((line = reader.readLine()) != null) {
            line = line.trim();
            if (line.isEmpty()) {
                continue;
            }

            // handle multiple data files in one statement
            Matcher matcher = dataFileRegex.matcher(line);
            String statement = matcher.replaceAll("\"" + tinysnbDir.resolve("$1").normalize() + "\"");

            try (QueryResult result = conn.query(statement)) {}
        }
        reader.close();


        try (QueryResult result = conn.query("create node table moviesSerial (ID SERIAL, name STRING, length INT32, note STRING, PRIMARY KEY (ID));")) {
        }

        try (QueryResult result = conn.query("copy moviesSerial from \"" +
                tinysnbSerialDir.resolve("vMovies.csv").normalize() + "\"")) {
        }
    }
}
