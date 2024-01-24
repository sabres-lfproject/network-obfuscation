/*
 * Create trivial network to test click/fastclick
 *
 * 	    x
 * 	    |
 * 	    a	// xor sender
 *    / | \
 *   t  u  v    // forwarders 
 *    \ | /
 *      b	// xor reciever
 *      |
 *      y	//
 *
 * Topology for testing multipath secret sharing.
 */

topo = {
    name: "xor_"+Math.random().toString().substr(-6),
    nodes: [
        ...["x", "y", "t", "u", "v"].map(x => node(x)),
	    ...["a", "b"].map(x => click(x)),
    ],
    switches: [],
    links: [
        v2v("x", 1, "a", 1, { mac: { x: '04:70:00:00:01:01', a: '04:70:00:00:00:01' } }),
        v2v("y", 1, "b", 1, { mac: { y: '04:70:00:00:02:01', b: '04:70:00:00:00:02' } }),

        v2v("a", 2, "t", 1, { mac: { a: '04:70:00:00:00:10', t: '04:70:00:00:03:10' } }),
        v2v("a", 3, "u", 1, { mac: { a: '04:70:00:00:00:20', u: '04:70:00:00:03:20' } }),
        v2v("a", 4, "v", 1, { mac: { a: '04:70:00:00:00:30', v: '04:70:00:00:03:30' } }),

        v2v("b", 2, "t", 2, { mac: { b: '04:70:00:00:00:11', t: '04:70:00:00:03:11' } }),
        v2v("b", 3, "u", 2, { mac: { b: '04:70:00:00:00:21', u: '04:70:00:00:03:21' } }),
        v2v("b", 4, "v", 2, { mac: { b: '04:70:00:00:00:31', v: '04:70:00:00:03:31' } }),
    ]
}

function node(name) {
    return {
        name: name,
        defaultnic: 'e1000',
        defaultdisktype: { dev: 'sda', bus: 'sata' },
        image: 'ubuntu-2004',
        mounts: [{ source: env.PWD, point: "/click" }], 
        cpu: { cores: 2 },
        memory: { capacity: GB(8) },
    };
}

function click(name) {
    return {
        name: name,
        defaultnic: 'e1000',
        defaultdisktype: { dev: 'sda', bus: 'sata' },
        image: 'ubuntu-2004',
        mounts: [{ source: env.PWD, point: "/click" }], 
        cpu: { cores: 4, passthru:true},
        memory: { capacity: GB(8) },
    };
}

function v2v(a, ai, b, bi, props={}) {
    lnk = Link(a, ai, b, bi, props);
    lnk.v2v = true;
    return lnk;
}
