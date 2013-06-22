prof
====

Profiling GASNet communicative operations in Cray Chapel.

1. Change $prog_name value in to desirable program name.
2. Execute run.sh. Information about puts/gets will be stored in ./results.
3. Execute graph.sh. Options, which are avaliable:
   -w "timeline width in pixels"
   -l "list of desirable locals e.g. 0 15 3 4"
4. Following files are generated:
   timeline.html - Exchanges between locales, color depends on module-invoker, width - exchenge time.
                   Longest operation is colored with red line.
   matrix.html - Locales-senders are in columns, recievers - in rows. Numbers in cells - number of exchanges,
                 greeb part of the cell - get operations, red - put operations. The more exchanges there were between
                 locales, the brighter segment will be.
   graph.html - Graph of exchanges between locales. Number and type of exchanges is shown.
   gathered_info - summed information about executed program.
