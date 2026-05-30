package com.ladybugdb

import org.junit.jupiter.api.Assertions.assertEquals
import org.junit.jupiter.api.Assertions.assertFalse
import org.junit.jupiter.api.Assertions.assertTrue
import org.junit.jupiter.api.Test

class KotlinUsageTest : TestBase() {
    @Test
    fun queryResultCanBeReadFromKotlin() {
        val result = TestHelper.getConnection().query("MATCH (p:person) RETURN p.fName ORDER BY p.ID LIMIT 2")
        try {
            assertTrue(result.isSuccess)
            assertEquals(1, result.numColumns)
            assertEquals("p.fName", result.getColumnName(0))

            val names = mutableListOf<String>()
            while (result.hasNext()) {
                val tuple = result.next
                names += tuple.getValue(0).getValue<String>()
            }

            assertEquals(listOf("Alice", "Bob"), names)
        } finally {
            result.close()
        }
    }

    @Test
    fun preparedStatementCanBeExecutedFromKotlin() {
        val statement = TestHelper.getConnection()
            .prepare("MATCH (p:person) WHERE p.fName = \$name RETURN COUNT(*)")
        try {
            assertTrue(statement.isSuccess)

            val params = mapOf("name" to Value("Alice"))
            val result = TestHelper.getConnection().execute(statement, params)
            try {
                assertTrue(result.isSuccess)
                assertTrue(result.hasNext())
                assertEquals(1L, result.next.getValue(0).getValue<Long>())
            } finally {
                params.values.forEach { it.close() }
                result.close()
            }
        } finally {
            statement.close()
        }
    }

    @Test
    fun valuesAndDataTypesAreUsableFromKotlin() {
        val value = Value(42L)
        try {
            assertFalse(value.isNull)
            assertEquals(42L, value.getValue<Long>())

            val dataType = value.dataType
            try {
                assertEquals(DataTypeID.INT64, dataType.id)
            } finally {
                dataType.close()
            }
        } finally {
            value.close()
        }
    }
}
