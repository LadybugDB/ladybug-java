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
    private static final Path datasetDir = findDatasetDir();
    private static final Path tinysnbDir = datasetDir.resolve("tinysnb");
    private static final Path tinysnbSerialDir = findTinysnbSerialDir(datasetDir);
    private final static String extensions = "csv|parquet|npy|ttl|nq|json|lbug_extension";
    private final static Pattern dataFileRegex = Pattern.compile("\"([^\"]+\\.(" + extensions + "))\"", Pattern.CASE_INSENSITIVE);

    private static Path findDatasetDir() {
        Path[] candidates = {
                projectDir.resolve("../../dataset").normalize(),
                projectDir.resolve("dataset").normalize(),
                projectDir.resolve("../go-ladybug/dataset").normalize(),
        };
        for (Path candidate : candidates) {
            if (candidate.resolve("tinysnb/schema.cypher").toFile().exists()) {
                return candidate;
            }
        }
        return candidates[0];
    }

    private static Path findTinysnbSerialDir(Path datasetDir) {
        Path serialDir = datasetDir.resolve("tinysnb-serial");
        if (serialDir.resolve("vMovies.csv").toFile().exists()) {
            return serialDir;
        }
        return datasetDir.resolve("tinysnb");
    }

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
                assertQuerySucceeded(result);
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
            String statement = matcher.replaceAll(match -> "\"" + resolveDataFile(match.group(1)) + "\"");

            try (QueryResult result = conn.query(statement)) {
                assertQuerySucceeded(result);
            }
        }
        reader.close();


        try (QueryResult result = conn.query("create node table moviesSerial (ID SERIAL, name STRING, length INT32, note STRING, PRIMARY KEY (ID));")) {
        }

        try (QueryResult result = conn.query("copy moviesSerial from \"" +
                tinysnbSerialDir.resolve("vMovies.csv").normalize() + "\"")) {
            assertQuerySucceeded(result);
        }
    }

    private static Path resolveDataFile(String fileName) {
        Path path = Paths.get(fileName);
        if (path.getNameCount() > 0 && "dataset".equals(path.getName(0).toString())) {
            return datasetDir.getParent().resolve(path).normalize();
        }
        return tinysnbDir.resolve(path).normalize();
    }

    private static void assertQuerySucceeded(QueryResult result) {
        if (!result.isSuccess()) {
            throw new RuntimeException(result.getErrorMessage());
        }
    }
}
