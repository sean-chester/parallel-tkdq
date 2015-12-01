echo 'Hybrid hybrid GGS PSkyline BSkyTree' > gpuPortResults.txt

# Run parallel scalability tests over varying thread counts
numactl --physcpubind=0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19 ~/SkylineBenchmark/bin/SkyBench -t "20" -f /data/schester/skyline_workloads/data-i-12-8000000.csv -s 'hybrid hybrid' >> hybrid.txt
numactl --physcpubind=0,1,2,3,4,5,6,7,8,9 ~/SkylineBenchmark/bin/SkyBench -t "10" -f /data/schester/skyline_workloads/data-i-12-8000000.csv -s 'hybrid hybrid' >> hybrid.txt
numactl --physcpubind=0,1,2,3,4,5,6,7 ~/SkylineBenchmark/bin/SkyBench -t "8" -f /data/schester/skyline_workloads/data-i-12-8000000.csv -s 'hybrid hybrid' >> hybrid.txt
numactl --physcpubind=0,1,2,3,4 ~/SkylineBenchmark/bin/SkyBench -t "5" -f /data/schester/skyline_workloads/data-i-12-8000000.csv -s 'hybrid hybrid' >> hybrid.txt
numactl --physcpubind=0,1,2,3 ~/SkylineBenchmark/bin/SkyBench -t "4" -f /data/schester/skyline_workloads/data-i-12-8000000.csv -s 'hybrid hybrid' >> hybrid.txt
numactl --physcpubind=0,1 ~/SkylineBenchmark/bin/SkyBench -t "2" -f /data/schester/skyline_workloads/data-i-12-8000000.csv -s 'hybrid hybrid' >> hybrid.txt
numactl --physcpubind=0 ~/SkylineBenchmark/bin/SkyBench -t "1" -f /data/schester/skyline_workloads/data-i-12-8000000.csv -s 'hybrid hybrid' >> hybrid.txt
numactl --physcpubind=0,2,4,6,8,10,12,14,16,18 ~/SkylineBenchmark/bin/SkyBench -t "10" -f /data/schester/skyline_workloads/data-i-12-8000000.csv -s 'hybrid hybrid' >> hybrid.txt
numactl --physcpubind=0,2,4,6,8,10,12,14 ~/SkylineBenchmark/bin/SkyBench -t "8" -f /data/schester/skyline_workloads/data-i-12-8000000.csv -s 'hybrid hybrid' >> hybrid.txt
numactl --physcpubind=0,2,4,6,8 ~/SkylineBenchmark/bin/SkyBench -t "5" -f /data/schester/skyline_workloads/data-i-12-8000000.csv -s 'hybrid hybrid' >> hybrid.txt
numactl --physcpubind=0,2,4,6 ~/SkylineBenchmark/bin/SkyBench -t "4" -f /data/schester/skyline_workloads/data-i-12-8000000.csv -s 'hybrid hybrid' >> hybrid.txt
numactl --physcpubind=0,2 ~/SkylineBenchmark/bin/SkyBench -t "2" -f /data/schester/skyline_workloads/data-i-12-8000000.csv -s 'hybrid hybrid' >> hybrid.txt


# Same as above, but this time measuring CPI.
numactl --physcpubind=0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19 ~/SkylineBenchmark/bin/SkyBench -t "20" -f /data/schester/skyline_workloads/data-i-12-8000000.csv -s 'hybrid' -p "throughput" >> hybrid.txt
numactl --physcpubind=0,1,2,3,4,5,6,7,8,9 ~/SkylineBenchmark/bin/SkyBench -t "10" -f /data/schester/skyline_workloads/data-i-12-8000000.csv -s 'hybrid' -p "throughput" >> hybrid.txt
numactl --physcpubind=0,1,2,3,4,5,6,7 ~/SkylineBenchmark/bin/SkyBench -t "8" -f /data/schester/skyline_workloads/data-i-12-8000000.csv -s 'hybrid' -p "throughput"  >> hybrid.txt
numactl --physcpubind=0,1,2,3,4 ~/SkylineBenchmark/bin/SkyBench -t "5" -f /data/schester/skyline_workloads/data-i-12-8000000.csv -s 'hybrid' -p "throughput" >> hybrid.txt
numactl --physcpubind=0,1,2,3 ~/SkylineBenchmark/bin/SkyBench -t "4" -f /data/schester/skyline_workloads/data-i-12-8000000.csv -s 'hybrid' -p "throughput"  >> hybrid.txt
numactl --physcpubind=0,1 ~/SkylineBenchmark/bin/SkyBench -t "2" -f /data/schester/skyline_workloads/data-i-12-8000000.csv -s 'hybrid' -p "throughput"  >> hybrid.txt
numactl --physcpubind=0 ~/SkylineBenchmark/bin/SkyBench -t "1" -f /data/schester/skyline_workloads/data-i-12-8000000.csv -s 'hybrid' -p "throughput"  >> hybrid.txt
numactl --physcpubind=0,2,4,6,8,10,12,14,16,18 ~/SkylineBenchmark/bin/SkyBench -t "10" -f /data/schester/skyline_workloads/data-i-12-8000000.csv -s 'hybrid' -p "throughput"  >> hybrid.txt
numactl --physcpubind=0,2,4,6,8,10,12,14 ~/SkylineBenchmark/bin/SkyBench -t "8" -f /data/schester/skyline_workloads/data-i-12-8000000.csv -s 'hybrid' -p "throughput"  >> hybrid.txt
numactl --physcpubind=0,2,4,6,8 ~/SkylineBenchmark/bin/SkyBench -t "5" -f /data/schester/skyline_workloads/data-i-12-8000000.csv -s 'hybrid' -p "throughput"  >> hybrid.txt
numactl --physcpubind=0,2,4,6 ~/SkylineBenchmark/bin/SkyBench -t "4" -f /data/schester/skyline_workloads/data-i-12-8000000.csv -s 'hybrid' -p "throughput"  >> hybrid.txt
numactl --physcpubind=0,2 ~/SkylineBenchmark/bin/SkyBench -t "2" -f /data/schester/skyline_workloads/data-i-12-8000000.csv -s 'hybrid' -p "throughput"  >> hybrid.txt


# Next, measure branch prediction across distributions
numactl --physcpubind=0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19 ~/SkylineBenchmark/bin/SkyBench -t "20" -f /data/schester/skyline_workloads/data-a-12-8000000.csv -s 'hybrid' -p "branch" >> hybrid.txt
numactl --physcpubind=0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19 ~/SkylineBenchmark/bin/SkyBench -t "20" -f /data/schester/skyline_workloads/data-i-12-8000000.csv -s 'hybrid' -p "branch" >> hybrid.txt
numactl --physcpubind=0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19 ~/SkylineBenchmark/bin/SkyBench -t "20" -f /data/schester/skyline_workloads/data-c-12-8000000.csv -s 'hybrid' -p "branch" >> hybrid.txt


# Then, measure cache ratios across distributions, dimensionality, and cardinality
numactl --physcpubind=0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19 ~/SkylineBenchmark/bin/SkyBench -t "20" -f /data/schester/skyline_workloads/data-a-12-8000000.csv -s 'hybrid' -p "cache" >> hybrid.txt
numactl --physcpubind=0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19 ~/SkylineBenchmark/bin/SkyBench -t "20" -f /data/schester/skyline_workloads/data-i-12-8000000.csv -s 'hybrid' -p "cache" >> hybrid.txt
numactl --physcpubind=0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19 ~/SkylineBenchmark/bin/SkyBench -t "20" -f /data/schester/skyline_workloads/data-c-12-8000000.csv -s 'hybrid' -p "cache" >> hybrid.txt

numactl --physcpubind=0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19 ~/SkylineBenchmark/bin/SkyBench -t "20" -f /data/schester/skyline_workloads/data-i-6-8000000.csv -s 'hybrid' -p "cache" >> hybrid.txt
numactl --physcpubind=0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19 ~/SkylineBenchmark/bin/SkyBench -t "20" -f /data/schester/skyline_workloads/data-i-16-8000000.csv -s 'hybrid' -p "cache" >> hybrid.txt

numactl --physcpubind=0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19 ~/SkylineBenchmark/bin/SkyBench -t "20" -f /data/schester/skyline_workloads/data-i-12-2000000.csv -s 'hybrid' -p "cache" >> hybrid.txt
numactl --physcpubind=0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19 ~/SkylineBenchmark/bin/SkyBench -t "20" -f /data/schester/skyline_workloads/data-i-12-128000000.csv -s 'hybrid' -p "cache" >> hybrid.txt

