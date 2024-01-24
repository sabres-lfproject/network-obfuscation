FromDPDKDevice(0000:02:00.0, N_QUEUES 1, MAXTHREADS 1, MODE none)
	->Print('x')
	->EtherRewrite(04:70:00:00:00:11, 04:70:00:00:03:11)
	->ToDPDKDevice(0000:03:00.0, N_QUEUES 1);
FromDPDKDevice(0000:03:00.0, N_QUEUES 1, MAXTHREADS 1, MODE none)
	->Print('y')
	->EtherRewrite(04:70:00:00:00:01, 04:70:00:00:02:01)
	->ToDPDKDevice(0000:02:00.0, N_QUEUES 1);
