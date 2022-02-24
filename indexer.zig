const std = @import("std");
const fs = std.fs;
const io = std.io;
const tokenize = std.mem.tokenize;

pub fn main() anyerror!void {

    // Read and write stream to stdin and stout
    const r_stream = io.bufferedReader(std.io.getStdIn().reader()).reader();
    const w_stream = io.bufferedWriter(std.io.getStdOut().writer()).writer();

    var r_buffer: [512]u8 = undefined; // Buffer for reading

    var current_word: []const u8 = "0";

    // Buffer size counter for less frequent flushing for optimiziation
    var buffer_size: usize = 0;

    while(try r_stream.readUntilDelimiterOrEof(&r_buffer, '\n')) |line| { // Read line
        var iterator = tokenize(u8, line, " ");

        var word = iterator.next().?[0..];

        if (!std.mem.eql(u8, word, current_word)) { // Word is new!
            if (!std.mem.eql(u8, current_word, "0")) { // Add new line on all but first line
                buffer_size += try w_stream.write("\n");
            }

            // Update word
            buffer_size += try w_stream.write(word);
            current_word = word;
        }

        // Write index
        buffer_size += try w_stream.write(" ");
        var val_slice = iterator.next().?;
        var value = val_slice[0..val_slice.len - 1];
        buffer_size += try w_stream.write(value);

        // Flush when the 4096 buffer is almost full
        if (buffer_size > 4000) {
            _ = try w_stream.context.flush();
            buffer_size = 0;
        }
    }
}