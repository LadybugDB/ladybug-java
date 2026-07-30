package com.ladybugdb;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.*;

import java.util.*;
import java.util.stream.Collectors;

public class PreparedStatementTest extends TestBase {

    @Test
    void PrepStmtIsSuccess() {
        String query = "MATCH (a:person) WHERE a.isStudent = $1 RETURN COUNT(*)";
        try (PreparedStatement preparedStatement1 = conn.prepare(query)) {
            assertNotNull(preparedStatement1);
            assertTrue(preparedStatement1.isSuccess());
        }

        query = "MATCH (a:personnnn) WHERE a.isStudent = $1 RETURN COUNT(*)";
        try (PreparedStatement preparedStatement2 = conn.prepare(query)) {
            assertNotNull(preparedStatement2);
            assertFalse(preparedStatement2.isSuccess());
        }
    }

    @Test
    void PrepStmtGetErrorMessage() {
        String query = "MATCH (a:person) WHERE a.isStudent = $1 RETURN COUNT(*)";
        try (PreparedStatement preparedStatement1 = conn.prepare(query)) {
            assertNotNull(preparedStatement1);
            String message = preparedStatement1.getErrorMessage();
            assertTrue(message.equals(""));
        }

        query = "MATCH (a:personnnn) WHERE a.isStudent = $1 RETURN COUNT(*)";
        try (PreparedStatement preparedStatement2 = conn.prepare(query)) {
            assertNotNull(preparedStatement2);
            String message = preparedStatement2.getErrorMessage();
            assertTrue(message.equals("Binder exception: Table personnnn does not exist."));
        }
    }

    @Test
    void PrepStmtSpecialString() {
        String query1 = "MATCH (n:movies) WHERE n.name = 'The 😂😃🧘🏻‍♂️🌍🌦️🍞🚗 movie' RETURN n.name";
        try (PreparedStatement preparedStatement1 = conn.prepare(query1)) {
            QueryResult result = conn.execute(preparedStatement1, Map.of());
            while (result.hasNext()) {
                String got = result.getNext().getValue(0).getValue();
                assertTrue(got.equals("The 😂😃🧘🏻‍♂️🌍🌦️🍞🚗 movie"));
            }
        }
        String query2 = "MATCH (n:movies) WHERE n.name = $1 RETURN n.name";
        Map<String, Value> params = Map.of("1", new Value("The 😂😃🧘🏻‍♂️🌍🌦️🍞🚗 movie"));
        try (PreparedStatement preparedStatement2 = conn.prepare(query2)) {
            QueryResult result = conn.execute(preparedStatement2, params);
            while (result.hasNext()) {
                String got = result.getNext().getValue(0).getValue();
                assertTrue(got.equals("The 😂😃🧘🏻‍♂️🌍🌦️🍞🚗 movie"));
            }
        }
    }

    @Test
    void executeWithRawBoxedString() {
        // Option A: raw String value auto-converted by C++ layer
        String query = "MATCH (n:person) WHERE n.fName = $1 RETURN n.fName";
        try (PreparedStatement ps = conn.prepare(query)) {
            Map<String, Object> raw = new HashMap<>();
            raw.put("1", "Alice");
            @SuppressWarnings("unchecked")
            Map<String, ?> params = (Map<String, ?>) (Map<?, ?>) raw;
            QueryResult result = conn.execute(ps, params);
            assertTrue(result.isSuccess());
            assertTrue(result.hasNext());
            String got = result.getNext().getValue(0).getValue();
            assertEquals("Alice", got);
        }
    }

    @Test
    void executeWithRawBoxedLong() {
        // Option A: raw Long value auto-converted by C++ layer
        String query = "MATCH (n:person) WHERE n.ID = $1 RETURN n.fName";
        try (PreparedStatement ps = conn.prepare(query)) {
            Map<String, Object> raw = new HashMap<>();
            raw.put("1", 0L);
            @SuppressWarnings("unchecked")
            Map<String, ?> params = (Map<String, ?>) (Map<?, ?>) raw;
            QueryResult result = conn.execute(ps, params);
            assertTrue(result.isSuccess());
            assertTrue(result.hasNext());
            String got = result.getNext().getValue(0).getValue();
            assertEquals("Alice", got);
        }
    }

    @Test
    void executeWithRawBoxedDouble() {
        // Option A: raw Double value auto-converted
        String query = "MATCH (n:person) WHERE n.eyeSight = $1 RETURN n.fName";
        try (PreparedStatement ps = conn.prepare(query)) {
            Map<String, Object> raw = new HashMap<>();
            raw.put("1", 5.0);
            @SuppressWarnings("unchecked")
            Map<String, ?> params = (Map<String, ?>) (Map<?, ?>) raw;
            QueryResult result = conn.execute(ps, params);
            assertTrue(result.isSuccess());
            assertTrue(result.hasNext());
            String got = result.getNext().getValue(0).getValue();
            assertEquals("Alice", got);
        }
    }

    @Test
    void executeWithRawBoxedBoolean() {
        // Option A: raw Boolean value auto-converted
        String query = "MATCH (n:person) WHERE n.isStudent = $1 RETURN n.fName";
        try (PreparedStatement ps = conn.prepare(query)) {
            Map<String, Object> raw = new HashMap<>();
            raw.put("1", true);
            @SuppressWarnings("unchecked")
            Map<String, ?> params = (Map<String, ?>) (Map<?, ?>) raw;
            QueryResult result = conn.execute(ps, params);
            assertTrue(result.isSuccess());
            assertTrue(result.hasNext());
        }
    }

    @Test
    void executeWithUnsupportedTypeThrows() {
        // Option B: unsupported type throws IllegalArgumentException instead of crashing
        String query = "MATCH (n:person) WHERE n.fName = $1 RETURN n.fName";
        try (PreparedStatement ps = conn.prepare(query)) {
            Map<String, Object> raw = new HashMap<>();
            raw.put("1", new ArrayList<>(List.of("not", "supported")));
            @SuppressWarnings("unchecked")
            Map<String, ?> params = (Map<String, ?>) (Map<?, ?>) raw;
            assertThrows(IllegalArgumentException.class, () -> {
                conn.execute(ps, params);
            });
        }
    }

    @Test
    void executeWithValueWrappedParamsRegression() {
        // Regression: Value-wrapped path is unchanged
        String query = "MATCH (n:person) WHERE n.fName = $1 RETURN n.fName";
        try (PreparedStatement ps = conn.prepare(query)) {
            Map<String, Value> params = Map.of("1", new Value("Alice"));
            QueryResult result = conn.execute(ps, params);
            assertTrue(result.isSuccess());
            assertTrue(result.hasNext());
            String got = result.getNext().getValue(0).getValue();
            assertEquals("Alice", got);
        }
    }

    @Test
    void executeWithMixedRawAndValueParams() {
        // Option A: mixed raw and Value-wrapped params
        String query = "MATCH (n:person) WHERE n.fName = $1 AND n.age = $2 RETURN n.fName";
        try (PreparedStatement ps = conn.prepare(query)) {
            Map<String, Object> raw = new HashMap<>();
            raw.put("1", new Value("Alice"));
            raw.put("2", 35L);
            @SuppressWarnings("unchecked")
            Map<String, ?> params = (Map<String, ?>) (Map<?, ?>) raw;
            QueryResult result = conn.execute(ps, params);
            assertTrue(result.isSuccess());
            assertTrue(result.hasNext());
        }
    }

}
