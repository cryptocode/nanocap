// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

(function (root, factory) {
  if (typeof define === 'function' && define.amd) {
    define(['kaitai-struct/KaitaiStream'], factory);
  } else if (typeof module === 'object' && module.exports) {
    module.exports = factory(require('kaitai-struct/KaitaiStream'));
  } else {
    root.Nano = factory(root.KaitaiStream);
  }
}(this, function (KaitaiStream) {
var Nano = (function() {
  Nano.EnumBlocktype = Object.freeze({
    INVALID: 0,
    NOT_A_BLOCK: 1,
    SEND: 2,
    RECEIVE: 3,
    OPEN: 4,
    CHANGE: 5,
    STATE: 6,

    0: "INVALID",
    1: "NOT_A_BLOCK",
    2: "SEND",
    3: "RECEIVE",
    4: "OPEN",
    5: "CHANGE",
    6: "STATE",
  });

  Nano.EnumMsgtype = Object.freeze({
    INVALID: 0,
    NOT_A_TYPE: 1,
    KEEPALIVE: 2,
    PUBLISH: 3,
    CONFIRM_REQ: 4,
    CONFIRM_ACK: 5,
    BULK_PULL: 6,
    BULK_PUSH: 7,
    FRONTIER_REQ: 8,
    NODE_ID_HANDSHAKE: 10,
    BULK_PULL_ACCOUNT: 11,
    TELEMETRY_REQ: 12,
    TELEMETRY_ACK: 13,
    ASC_PULL_REQ: 14,
    ASC_PULL_ACK: 15,

    0: "INVALID",
    1: "NOT_A_TYPE",
    2: "KEEPALIVE",
    3: "PUBLISH",
    4: "CONFIRM_REQ",
    5: "CONFIRM_ACK",
    6: "BULK_PULL",
    7: "BULK_PUSH",
    8: "FRONTIER_REQ",
    10: "NODE_ID_HANDSHAKE",
    11: "BULK_PULL_ACCOUNT",
    12: "TELEMETRY_REQ",
    13: "TELEMETRY_ACK",
    14: "ASC_PULL_REQ",
    15: "ASC_PULL_ACK",
  });

  Nano.EnumNetwork = Object.freeze({
    NETWORK_TEST: 65,
    NETWORK_BETA: 66,
    NETWORK_LIVE: 67,

    65: "NETWORK_TEST",
    66: "NETWORK_BETA",
    67: "NETWORK_LIVE",
  });

  Nano.EnumAscHashType = Object.freeze({
    ACCOUNT_HASH_TYPE: 0,
    BLOCK_HASH_TYPE: 1,

    0: "ACCOUNT_HASH_TYPE",
    1: "BLOCK_HASH_TYPE",
  });

  Nano.EnumBulkPullAccount = Object.freeze({
    PENDING_HASH_AND_AMOUNT: 0,
    PENDING_ADDRESS_ONLY: 1,
    PENDING_HASH_AMOUNT_AND_ADDRESS: 2,

    0: "PENDING_HASH_AND_AMOUNT",
    1: "PENDING_ADDRESS_ONLY",
    2: "PENDING_HASH_AMOUNT_AND_ADDRESS",
  });

  Nano.ProtocolVersion = Object.freeze({
    VALUE: 19,

    19: "VALUE",
  });

  Nano.EnumAscPullType = Object.freeze({
    INVALID: 0,
    BLOCK_PULL_TYPE: 1,
    ACCOUNT_PULL_TYPE: 2,

    0: "INVALID",
    1: "BLOCK_PULL_TYPE",
    2: "ACCOUNT_PULL_TYPE",
  });

  function Nano(_io, _parent, _root) {
    this._io = _io;
    this._parent = _parent;
    this._root = _root || this;

    this._read();
  }
  Nano.prototype._read = function() {
    this.header = new MessageHeader(this._io, this, this._root);
    switch (this.header.messageType) {
    case Nano.EnumMsgtype.CONFIRM_REQ:
      this.body = new MsgConfirmReq(this._io, this, this._root);
      break;
    case Nano.EnumMsgtype.CONFIRM_ACK:
      this.body = new MsgConfirmAck(this._io, this, this._root);
      break;
    case Nano.EnumMsgtype.BULK_PULL:
      this.body = new MsgBulkPull(this._io, this, this._root);
      break;
    case Nano.EnumMsgtype.BULK_PULL_ACCOUNT:
      this.body = new MsgBulkPullAccount(this._io, this, this._root);
      break;
    case Nano.EnumMsgtype.KEEPALIVE:
      this.body = new MsgKeepalive(this._io, this, this._root);
      break;
    case Nano.EnumMsgtype.ASC_PULL_ACK:
      this.body = new MsgAscPullAck(this._io, this, this._root);
      break;
    case Nano.EnumMsgtype.BULK_PUSH:
      this.body = new MsgBulkPush(this._io, this, this._root);
      break;
    case Nano.EnumMsgtype.TELEMETRY_ACK:
      this.body = new MsgTelemetryAck(this._io, this, this._root);
      break;
    case Nano.EnumMsgtype.NODE_ID_HANDSHAKE:
      this.body = new MsgNodeIdHandshake(this._io, this, this._root);
      break;
    case Nano.EnumMsgtype.FRONTIER_REQ:
      this.body = new MsgFrontierReq(this._io, this, this._root);
      break;
    case Nano.EnumMsgtype.PUBLISH:
      this.body = new MsgPublish(this._io, this, this._root);
      break;
    case Nano.EnumMsgtype.TELEMETRY_REQ:
      this.body = new MsgTelemetryReq(this._io, this, this._root);
      break;
    case Nano.EnumMsgtype.ASC_PULL_REQ:
      this.body = new MsgAscPullReq(this._io, this, this._root);
      break;
    default:
      this.body = new IgnoreUntilEof(this._io, this, this._root);
      break;
    }
  }

  var BlockSend = Nano.BlockSend = (function() {
    function BlockSend(_io, _parent, _root) {
      this._io = _io;
      this._parent = _parent;
      this._root = _root || this;

      this._read();
    }
    BlockSend.prototype._read = function() {
      this.previous = this._io.readBytes(32);
      this.destination = this._io.readBytes(32);
      this.balance = this._io.readBytes(16);
      this.signature = this._io.readBytes(64);
      this.work = this._io.readU8le();
    }

    /**
     * Hash of the previous block
     */

    /**
     * Public key of destination account
     */

    /**
     * 128-bit big endian balance
     */

    /**
     * ed25519-blake2b signature
     */

    /**
     * Proof of work
     */

    return BlockSend;
  })();

  /**
   * A sequence of hash,root pairs
   */

  var ConfirmRequestByHash = Nano.ConfirmRequestByHash = (function() {
    function ConfirmRequestByHash(_io, _parent, _root) {
      this._io = _io;
      this._parent = _parent;
      this._root = _root || this;

      this._read();
    }
    ConfirmRequestByHash.prototype._read = function() {
      if (!(this._io.isEof())) {
        this.pairs = []
        var i = 0;
        do {
          var _ = new HashPair(this._io, this, this._root);
          this.pairs.push(_);
          i++;
        } while (!( ((i == this._root.header.itemCountInt) || (this._io.isEof())) ));
      }
    }

    /**
     * Up to "count" pairs of hash (first) and root (second), where count is read from header.
     */

    return ConfirmRequestByHash;
  })();

  /**
   * Request node telemetry metrics
   */

  var MsgTelemetryReq = Nano.MsgTelemetryReq = (function() {
    function MsgTelemetryReq(_io, _parent, _root) {
      this._io = _io;
      this._parent = _parent;
      this._root = _root || this;

      this._read();
    }
    MsgTelemetryReq.prototype._read = function() {
    }

    return MsgTelemetryReq;
  })();

  /**
   * Selects a block based on the argument
   */

  var BlockSelector = Nano.BlockSelector = (function() {
    function BlockSelector(_io, _parent, _root, argBlockType) {
      this._io = _io;
      this._parent = _parent;
      this._root = _root || this;
      this.argBlockType = argBlockType;

      this._read();
    }
    BlockSelector.prototype._read = function() {
      switch (this.argBlockType) {
      case Nano.EnumBlocktype.RECEIVE:
        this.block = new BlockReceive(this._io, this, this._root);
        break;
      case Nano.EnumBlocktype.CHANGE:
        this.block = new BlockChange(this._io, this, this._root);
        break;
      case Nano.EnumBlocktype.STATE:
        this.block = new BlockState(this._io, this, this._root);
        break;
      case Nano.EnumBlocktype.OPEN:
        this.block = new BlockOpen(this._io, this, this._root);
        break;
      case Nano.EnumBlocktype.SEND:
        this.block = new BlockSend(this._io, this, this._root);
        break;
      default:
        this.block = new IgnoreUntilEof(this._io, this, this._root);
        break;
      }
    }

    return BlockSelector;
  })();

  var NodeIdQuery = Nano.NodeIdQuery = (function() {
    function NodeIdQuery(_io, _parent, _root) {
      this._io = _io;
      this._parent = _parent;
      this._root = _root || this;

      this._read();
    }
    NodeIdQuery.prototype._read = function() {
      this.cookie = this._io.readBytes(32);
    }

    /**
     * Per-endpoint random number
     */

    return NodeIdQuery;
  })();

  var BlockReceive = Nano.BlockReceive = (function() {
    function BlockReceive(_io, _parent, _root) {
      this._io = _io;
      this._parent = _parent;
      this._root = _root || this;

      this._read();
    }
    BlockReceive.prototype._read = function() {
      this.previous = this._io.readBytes(32);
      this.source = this._io.readBytes(32);
      this.signature = this._io.readBytes(64);
      this.work = this._io.readU8le();
    }

    /**
     * Hash of the previous block
     */

    /**
     * Hash of the source send block
     */

    /**
     * ed25519-blake2b signature
     */

    /**
     * Proof of work
     */

    return BlockReceive;
  })();

  var BlockChange = Nano.BlockChange = (function() {
    function BlockChange(_io, _parent, _root) {
      this._io = _io;
      this._parent = _parent;
      this._root = _root || this;

      this._read();
    }
    BlockChange.prototype._read = function() {
      this.previous = this._io.readBytes(32);
      this.representative = this._io.readBytes(32);
      this.signature = this._io.readBytes(64);
      this.work = this._io.readU8le();
    }

    /**
     * Hash of the previous block
     */

    /**
     * Public key of new representative account
     */

    /**
     * ed25519-blake2b signature
     */

    /**
     * Proof of work
     */

    return BlockChange;
  })();

  /**
   * Bulk pull request.
   */

  var MsgBulkPull = Nano.MsgBulkPull = (function() {
    function MsgBulkPull(_io, _parent, _root) {
      this._io = _io;
      this._parent = _parent;
      this._root = _root || this;

      this._read();
    }
    MsgBulkPull.prototype._read = function() {
      this.start = this._io.readBytes(32);
      this.end = this._io.readBytes(32);
      if (this._root.header.extendedParamsPresent != 0) {
        this.extended = new ExtendedParameters(this._io, this, this._root);
      }
    }

    var ExtendedParameters = MsgBulkPull.ExtendedParameters = (function() {
      function ExtendedParameters(_io, _parent, _root) {
        this._io = _io;
        this._parent = _parent;
        this._root = _root || this;

        this._read();
      }
      ExtendedParameters.prototype._read = function() {
        this.zero = this._io.readU1();
        this.count = this._io.readU4le();
        this.reserved = this._io.readBytes(3);
      }

      /**
       * Must be 0
       */

      /**
       * little endian "count" parameter to limit the response set.
       */

      /**
       * Reserved extended parameter bytes
       */

      return ExtendedParameters;
    })();

    /**
     * Account public key or block hash.
     */

    /**
     * End block hash. May be zero.
     */

    return MsgBulkPull;
  })();

  /**
   * A peer entry in the keepalive message
   */

  var Peer = Nano.Peer = (function() {
    function Peer(_io, _parent, _root) {
      this._io = _io;
      this._parent = _parent;
      this._root = _root || this;

      this._read();
    }
    Peer.prototype._read = function() {
      this.address = this._io.readBytes(16);
      this.port = this._io.readU2le();
    }

    /**
     * ipv6 address, or ipv6-mapped ipv4 address.
     */

    /**
     * Port number. Default port is 7075.
     */

    return Peer;
  })();

  /**
   * Ascending pull sub-header, data common in asc pull packets.
   */

  var AscPullBase = Nano.AscPullBase = (function() {
    function AscPullBase(_io, _parent, _root) {
      this._io = _io;
      this._parent = _parent;
      this._root = _root || this;

      this._read();
    }
    AscPullBase.prototype._read = function() {
      this.type = this._io.readU1();
      this.id = this._io.readU8be();
    }

    /**
     * An identifier to associate replies with requests
     */

    return AscPullBase;
  })();

  var BlockOpen = Nano.BlockOpen = (function() {
    function BlockOpen(_io, _parent, _root) {
      this._io = _io;
      this._parent = _parent;
      this._root = _root || this;

      this._read();
    }
    BlockOpen.prototype._read = function() {
      this.source = this._io.readBytes(32);
      this.representative = this._io.readBytes(32);
      this.account = this._io.readBytes(32);
      this.signature = this._io.readBytes(64);
      this.work = this._io.readU8le();
    }

    /**
     * Hash of the source send block
     */

    /**
     * Public key of initial representative account
     */

    /**
     * Public key of account being opened
     */

    /**
     * ed25519-blake2b signature
     */

    /**
     * Proof of work
     */

    return BlockOpen;
  })();

  var IgnoreUntilEof = Nano.IgnoreUntilEof = (function() {
    function IgnoreUntilEof(_io, _parent, _root) {
      this._io = _io;
      this._parent = _parent;
      this._root = _root || this;

      this._read();
    }
    IgnoreUntilEof.prototype._read = function() {
      if (!(this._io.isEof())) {
        this.empty = []
        var i = 0;
        do {
          var _ = this._io.readU1();
          this.empty.push(_);
          i++;
        } while (!(this._io.isEof()));
      }
    }

    return IgnoreUntilEof;
  })();

  /**
   * Signed confirmation of a list of block hashes
   */

  var MsgConfirmAck = Nano.MsgConfirmAck = (function() {
    function MsgConfirmAck(_io, _parent, _root) {
      this._io = _io;
      this._parent = _parent;
      this._root = _root || this;

      this._read();
    }
    MsgConfirmAck.prototype._read = function() {
      this.common = new VoteCommon(this._io, this, this._root);
      this.votebyhash = new VoteByHash(this._io, this, this._root);
    }

    return MsgConfirmAck;
  })();

  /**
   * A node ID handshake request and/or response.
   */

  var MsgNodeIdHandshake = Nano.MsgNodeIdHandshake = (function() {
    function MsgNodeIdHandshake(_io, _parent, _root) {
      this._io = _io;
      this._parent = _parent;
      this._root = _root || this;

      this._read();
    }
    MsgNodeIdHandshake.prototype._read = function() {
      if (this._root.header.queryFlag != 0) {
        this.query = new NodeIdQuery(this._io, this, this._root);
      }
      if (this._root.header.responseFlag != 0) {
        this.response = new NodeIdResponse(this._io, this, this._root);
      }
    }

    return MsgNodeIdHandshake;
  })();

  /**
   * Request frontiers (account chain head blocks) from a remote node
   */

  var MsgFrontierReq = Nano.MsgFrontierReq = (function() {
    function MsgFrontierReq(_io, _parent, _root) {
      this._io = _io;
      this._parent = _parent;
      this._root = _root || this;

      this._read();
    }
    MsgFrontierReq.prototype._read = function() {
      this.start = this._io.readBytes(32);
      this.age = this._io.readU4le();
      this.count = this._io.readU4le();
    }

    /**
     * Public key of start account
     */

    /**
     * Maximum age of included account. If 0xffffffff, turn off age filtering.
     */

    /**
     * Maximum number of accounts to include. If 0xffffffff, turn off count limiting.
     */

    return MsgFrontierReq;
  })();

  /**
   * A bulk push is equivalent to an unsolicited bulk pull response.
   * If a node knows about an account a peer doesn't, the node sends
   * its local blocks for that account to the peer. The stream of
   * blocks ends with a sentinel block of type enum_blocktype::not_a_block.
   */

  var MsgBulkPush = Nano.MsgBulkPush = (function() {
    function MsgBulkPush(_io, _parent, _root) {
      this._io = _io;
      this._parent = _parent;
      this._root = _root || this;

      this._read();
    }
    MsgBulkPush.prototype._read = function() {
      this.entry = []
      var i = 0;
      do {
        var _ = new BulkPushEntry(this._io, this, this._root);
        this.entry.push(_);
        i++;
      } while (!( ((this._io.isEof()) || (this.entry[i].blockType == Nano.EnumBlocktype.NOT_A_BLOCK)) ));
    }

    var BulkPushEntry = MsgBulkPush.BulkPushEntry = (function() {
      function BulkPushEntry(_io, _parent, _root) {
        this._io = _io;
        this._parent = _parent;
        this._root = _root || this;

        this._read();
      }
      BulkPushEntry.prototype._read = function() {
        this.blockType = this._io.readU1();
        this.block = new BlockSelector(this._io, this, this._root, this.blockType);
      }

      return BulkPushEntry;
    })();

    return MsgBulkPush;
  })();

  var NodeIdResponse = Nano.NodeIdResponse = (function() {
    function NodeIdResponse(_io, _parent, _root) {
      this._io = _io;
      this._parent = _parent;
      this._root = _root || this;

      this._read();
    }
    NodeIdResponse.prototype._read = function() {
      this.account = this._io.readBytes(32);
      this.signature = this._io.readBytes(64);
    }

    /**
     * Account (node id)
     */

    /**
     * Signature
     */

    return NodeIdResponse;
  })();

  /**
   * ascending pull response
   */

  var MsgAscPullAck = Nano.MsgAscPullAck = (function() {
    function MsgAscPullAck(_io, _parent, _root) {
      this._io = _io;
      this._parent = _parent;
      this._root = _root || this;

      this._read();
    }
    MsgAscPullAck.prototype._read = function() {
      this.base = new AscPullBase(this._io, this, this._root);
      this._raw_payload = this._io.readBytes(this._root.header.ascPullSize);
      var _io__raw_payload = new KaitaiStream(this._raw_payload);
      this.payload = new AscPullAckPayload(_io__raw_payload, this, this._root);
    }

    /**
     * ascend pull response account info payload
     */

    var AccountInfoAckPayload = MsgAscPullAck.AccountInfoAckPayload = (function() {
      function AccountInfoAckPayload(_io, _parent, _root) {
        this._io = _io;
        this._parent = _parent;
        this._root = _root || this;

        this._read();
      }
      AccountInfoAckPayload.prototype._read = function() {
        this.account = this._io.readBytes(32);
        this.accountOpen = this._io.readBytes(32);
        this.accountHead = this._io.readBytes(32);
        this.blockCount = this._io.readU8be();
        this.confFrontier = this._io.readBytes(32);
        this.confHeight = this._io.readU8be();
      }

      /**
       * account being pulled
       */

      /**
       * opening block of account
       */

      /**
       * block with highest height (can be confirmed or unconfirmed)
       */

      /**
       * number of blocks in account (counts confirmed and unconfirmed blocks)
       */

      /**
       * block hash of confirmation frontier, confirmed block with highest height
       */

      /**
       * Height of highest confirmed block
       */

      return AccountInfoAckPayload;
    })();

    /**
     * ascend pull response block payload
     */

    var BlocksAckPayload = MsgAscPullAck.BlocksAckPayload = (function() {
      function BlocksAckPayload(_io, _parent, _root) {
        this._io = _io;
        this._parent = _parent;
        this._root = _root || this;

        this._read();
      }
      BlocksAckPayload.prototype._read = function() {
        this.entry = []
        var i = 0;
        do {
          var _ = new AscPullEntry(this._io, this, this._root);
          this.entry.push(_);
          i++;
        } while (!( ((this._io.isEof()) || (_.blockType == Nano.EnumBlocktype.NOT_A_BLOCK)) ));
      }

      var AscPullEntry = BlocksAckPayload.AscPullEntry = (function() {
        function AscPullEntry(_io, _parent, _root) {
          this._io = _io;
          this._parent = _parent;
          this._root = _root || this;

          this._read();
        }
        AscPullEntry.prototype._read = function() {
          this.blockType = this._io.readU1();
          this.block = new BlockSelector(this._io, this, this._root, this.blockType);
        }

        return AscPullEntry;
      })();

      return BlocksAckPayload;
    })();

    /**
     * payload of ascend pull responses
     */

    var AscPullAckPayload = MsgAscPullAck.AscPullAckPayload = (function() {
      function AscPullAckPayload(_io, _parent, _root) {
        this._io = _io;
        this._parent = _parent;
        this._root = _root || this;

        this._read();
      }
      AscPullAckPayload.prototype._read = function() {
        if (this._parent.base.type == Nano.EnumAscPullType.ACCOUNT_PULL_TYPE) {
          this.accountInfo = new AccountInfoAckPayload(this._io, this, this._root);
        }
        if (this._parent.base.type == Nano.EnumAscPullType.BLOCK_PULL_TYPE) {
          this.blocks = new BlocksAckPayload(this._io, this, this._root);
        }
      }

      return AscPullAckPayload;
    })();

    return MsgAscPullAck;
  })();

  /**
   * The msg_bulk_push request does not have a response.
   */

  var BulkPushResponse = Nano.BulkPushResponse = (function() {
    function BulkPushResponse(_io, _parent, _root) {
      this._io = _io;
      this._parent = _parent;
      this._root = _root || this;

      this._read();
    }
    BulkPushResponse.prototype._read = function() {
    }

    return BulkPushResponse;
  })();

  /**
   * Response of the msg_bulk_pull request.
   */

  var BulkPullResponse = Nano.BulkPullResponse = (function() {
    function BulkPullResponse(_io, _parent, _root) {
      this._io = _io;
      this._parent = _parent;
      this._root = _root || this;

      this._read();
    }
    BulkPullResponse.prototype._read = function() {
      this.entry = []
      var i = 0;
      do {
        var _ = new BulkPullEntry(this._io, this, this._root);
        this.entry.push(_);
        i++;
      } while (!( ((this._io.isEof()) || (this.entry[i].blockType == Nano.EnumBlocktype.NOT_A_BLOCK)) ));
    }

    var BulkPullEntry = BulkPullResponse.BulkPullEntry = (function() {
      function BulkPullEntry(_io, _parent, _root) {
        this._io = _io;
        this._parent = _parent;
        this._root = _root || this;

        this._read();
      }
      BulkPullEntry.prototype._read = function() {
        this.blockType = this._io.readU1();
        this.block = new BlockSelector(this._io, this, this._root, this.blockType);
      }

      return BulkPullEntry;
    })();

    return BulkPullResponse;
  })();

  /**
   * Publish the given block
   */

  var MsgPublish = Nano.MsgPublish = (function() {
    function MsgPublish(_io, _parent, _root) {
      this._io = _io;
      this._parent = _parent;
      this._root = _root || this;

      this._read();
    }
    MsgPublish.prototype._read = function() {
      this.body = new BlockSelector(this._io, this, this._root, this._root.header.blockTypeInt);
    }

    return MsgPublish;
  })();

  /**
   * State block
   */

  var BlockState = Nano.BlockState = (function() {
    function BlockState(_io, _parent, _root) {
      this._io = _io;
      this._parent = _parent;
      this._root = _root || this;

      this._read();
    }
    BlockState.prototype._read = function() {
      this.account = this._io.readBytes(32);
      this.previous = this._io.readBytes(32);
      this.representative = this._io.readBytes(32);
      this.balance = this._io.readBytes(16);
      this.link = this._io.readBytes(32);
      this.signature = this._io.readBytes(64);
      this.work = this._io.readU8be();
    }

    /**
     * Public key of account represented by this state block
     */

    /**
     * Hash of previous block
     */

    /**
     * Public key of the representative account
     */

    /**
     * 128-bit big endian balance
     */

    /**
     * Pairing send's block hash (open/receive), 0 (change) or destination public key (send)
     */

    /**
     * ed25519-blake2b signature
     */

    /**
     * Proof of work (big endian)
     */

    return BlockState;
  })();

  /**
   * A general purpose pair of 32-byte hash values
   */

  var HashPair = Nano.HashPair = (function() {
    function HashPair(_io, _parent, _root) {
      this._io = _io;
      this._parent = _parent;
      this._root = _root || this;

      this._read();
    }
    HashPair.prototype._read = function() {
      this.first = this._io.readBytes(32);
      this.second = this._io.readBytes(32);
    }

    /**
     * First hash in pair
     */

    /**
     * Second hash in pair
     */

    return HashPair;
  })();

  /**
   * Response of the msg_bulk_pull_account message. The structure depends on the
   * flags that was passed to the query.
   */

  var BulkPullAccountResponse = Nano.BulkPullAccountResponse = (function() {
    function BulkPullAccountResponse(_io, _parent, _root, flags) {
      this._io = _io;
      this._parent = _parent;
      this._root = _root || this;
      this.flags = flags;

      this._read();
    }
    BulkPullAccountResponse.prototype._read = function() {
      this.frontierEntry = new FrontierBalanceEntry(this._io, this, this._root);
      this.pendingEntry = []
      var i = 0;
      do {
        var _ = new BulkPullAccountEntry(this._io, this, this._root, this.flags);
        this.pendingEntry.push(_);
        i++;
      } while (!( ((this._io.isEof()) || ((KaitaiStream.byteArrayCompare(this.pendingEntry[i].hash, this._root.constBlockZero) == 0))) ));
    }

    var FrontierBalanceEntry = BulkPullAccountResponse.FrontierBalanceEntry = (function() {
      function FrontierBalanceEntry(_io, _parent, _root) {
        this._io = _io;
        this._parent = _parent;
        this._root = _root || this;

        this._read();
      }
      FrontierBalanceEntry.prototype._read = function() {
        this.frontierHash = this._io.readBytes(32);
        this.balance = this._io.readBytes(16);
      }

      /**
       * Hash of the head block of the account chain.
       */

      /**
       * 128-bit big endian account balance.
       */

      return FrontierBalanceEntry;
    })();

    var BulkPullAccountEntry = BulkPullAccountResponse.BulkPullAccountEntry = (function() {
      function BulkPullAccountEntry(_io, _parent, _root, flags) {
        this._io = _io;
        this._parent = _parent;
        this._root = _root || this;
        this.flags = flags;

        this._read();
      }
      BulkPullAccountEntry.prototype._read = function() {
        if (!(this.pendingAddressOnly)) {
          this.hash = this._io.readBytes(32);
        }
        if (!(this.pendingAddressOnly)) {
          this.amount = this._io.readBytes(16);
        }
        if ( ((this.pendingAddressOnly) || (this.pendingIncludeAddress)) ) {
          this.source = this._io.readBytes(32);
        }
      }
      Object.defineProperty(BulkPullAccountEntry.prototype, 'pendingAddressOnly', {
        get: function() {
          if (this._m_pendingAddressOnly !== undefined)
            return this._m_pendingAddressOnly;
          this._m_pendingAddressOnly = this.flags == Nano.EnumBulkPullAccount.PENDING_ADDRESS_ONLY;
          return this._m_pendingAddressOnly;
        }
      });
      Object.defineProperty(BulkPullAccountEntry.prototype, 'pendingIncludeAddress', {
        get: function() {
          if (this._m_pendingIncludeAddress !== undefined)
            return this._m_pendingIncludeAddress;
          this._m_pendingIncludeAddress = this.flags == Nano.EnumBulkPullAccount.PENDING_HASH_AMOUNT_AND_ADDRESS;
          return this._m_pendingIncludeAddress;
        }
      });

      return BulkPullAccountEntry;
    })();

    return BulkPullAccountResponse;
  })();

  /**
   * A list of 8 peers, some of which may be all-zero.
   */

  var MsgKeepalive = Nano.MsgKeepalive = (function() {
    function MsgKeepalive(_io, _parent, _root) {
      this._io = _io;
      this._parent = _parent;
      this._root = _root || this;

      this._read();
    }
    MsgKeepalive.prototype._read = function() {
      if (!(this._io.isEof())) {
        this.peers = []
        var i = 0;
        do {
          var _ = new Peer(this._io, this, this._root);
          this.peers.push(_);
          i++;
        } while (!( ((i == 8) || (this._io.isEof())) ));
      }
    }

    return MsgKeepalive;
  })();

  /**
   * Requests confirmation of the given block or list of root/hash pairs
   */

  var MsgConfirmReq = Nano.MsgConfirmReq = (function() {
    function MsgConfirmReq(_io, _parent, _root) {
      this._io = _io;
      this._parent = _parent;
      this._root = _root || this;

      this._read();
    }
    MsgConfirmReq.prototype._read = function() {
      if (this._root.header.blockType == Nano.EnumBlocktype.NOT_A_BLOCK) {
        this.reqbyhash = new ConfirmRequestByHash(this._io, this, this._root);
      }
      if (this._root.header.blockType != Nano.EnumBlocktype.NOT_A_BLOCK) {
        this.block = new BlockSelector(this._io, this, this._root, this._root.header.blockTypeInt);
      }
    }

    return MsgConfirmReq;
  })();

  /**
   * Bulk pull account request.
   */

  var MsgBulkPullAccount = Nano.MsgBulkPullAccount = (function() {
    function MsgBulkPullAccount(_io, _parent, _root) {
      this._io = _io;
      this._parent = _parent;
      this._root = _root || this;

      this._read();
    }
    MsgBulkPullAccount.prototype._read = function() {
      this.account = this._io.readBytes(32);
      this.minimumAmount = this._io.readBytes(16);
      this.flags = this._io.readU1();
    }

    /**
     * Account public key.
     */

    /**
     * 128-bit big endian minimum amount.
     */

    return MsgBulkPullAccount;
  })();

  /**
   * A sequence of hashes, where count is read from header.
   */

  var VoteByHash = Nano.VoteByHash = (function() {
    function VoteByHash(_io, _parent, _root) {
      this._io = _io;
      this._parent = _parent;
      this._root = _root || this;

      this._read();
    }
    VoteByHash.prototype._read = function() {
      if (!(this._io.isEof())) {
        this.hashes = []
        var i = 0;
        do {
          var _ = this._io.readBytes(32);
          this.hashes.push(_);
          i++;
        } while (!( ((i == this._root.header.itemCountInt) || (this._io.isEof())) ));
      }
    }

    return VoteByHash;
  })();

  /**
   * Common data shared by votes
   */

  var VoteCommon = Nano.VoteCommon = (function() {
    function VoteCommon(_io, _parent, _root) {
      this._io = _io;
      this._parent = _parent;
      this._root = _root || this;

      this._read();
    }
    VoteCommon.prototype._read = function() {
      this.account = this._io.readBytes(32);
      this.signature = this._io.readBytes(64);
      this.timestampAndVoteDuration = this._io.readU8le();
    }

    /**
     * Number of seconds since the UTC epoch vote was generated at
     */
    Object.defineProperty(VoteCommon.prototype, 'timestamp', {
      get: function() {
        if (this._m_timestamp !== undefined)
          return this._m_timestamp;
        this._m_timestamp = (this.timestampAndVoteDuration & 18446744073709551600);
        return this._m_timestamp;
      }
    });

    /**
     * Since V23.0 this is specified as 2^(duration + 4) in milliseconds
     */
    Object.defineProperty(VoteCommon.prototype, 'voteDurationBits', {
      get: function() {
        if (this._m_voteDurationBits !== undefined)
          return this._m_voteDurationBits;
        this._m_voteDurationBits = (this.timestampAndVoteDuration & 15);
        return this._m_voteDurationBits;
      }
    });

    return VoteCommon;
  })();

  var MessageHeader = Nano.MessageHeader = (function() {
    function MessageHeader(_io, _parent, _root) {
      this._io = _io;
      this._parent = _parent;
      this._root = _root || this;

      this._read();
    }
    MessageHeader.prototype._read = function() {
      this.magic = this._io.readBytes(1);
      if (!((KaitaiStream.byteArrayCompare(this.magic, [82]) == 0))) {
        throw new KaitaiStream.ValidationNotEqualError([82], this.magic, this._io, "/types/message_header/seq/0");
      }
      this.networkId = this._io.readU1();
      this.versionMax = this._io.readU1();
      this.versionUsing = this._io.readU1();
      this.versionMin = this._io.readU1();
      this.messageType = this._io.readU1();
      this.extensions = this._io.readU2le();
    }

    /**
     * Since protocol version 19 (release 24).
     * May be set for "bulk_pull" messages.
     * If set, it reverses the order in which bulk_pull returns blocks. It returns the frontier block last.
     */
    Object.defineProperty(MessageHeader.prototype, 'bulkPullAscendingFlag', {
      get: function() {
        if (this._m_bulkPullAscendingFlag !== undefined)
          return this._m_bulkPullAscendingFlag;
        this._m_bulkPullAscendingFlag = (this.extensions & 2);
        return this._m_bulkPullAscendingFlag;
      }
    });

    /**
     * If set, this is a node_id_handshake response. This maybe be set at the
     * same time as the query_flag.
     */
    Object.defineProperty(MessageHeader.prototype, 'responseFlag', {
      get: function() {
        if (this._m_responseFlag !== undefined)
          return this._m_responseFlag;
        this._m_responseFlag = (this.extensions & 2);
        return this._m_responseFlag;
      }
    });
    Object.defineProperty(MessageHeader.prototype, 'blockTypeInt', {
      get: function() {
        if (this._m_blockTypeInt !== undefined)
          return this._m_blockTypeInt;
        this._m_blockTypeInt = ((this.extensions & 3840) >>> 8);
        return this._m_blockTypeInt;
      }
    });

    /**
     * Since protocol version 18.
     * Must be set for "telemetry_ack" messages. Indicates size of payload.
     */
    Object.defineProperty(MessageHeader.prototype, 'telemetrySize', {
      get: function() {
        if (this._m_telemetrySize !== undefined)
          return this._m_telemetrySize;
        this._m_telemetrySize = (this.extensions & 1023);
        return this._m_telemetrySize;
      }
    });

    /**
     * Since protocol version 15.
     * May be set for "bulk_pull" messages.
     * If set, the bulk_pull message contain extended parameters.
     */
    Object.defineProperty(MessageHeader.prototype, 'extendedParamsPresent', {
      get: function() {
        if (this._m_extendedParamsPresent !== undefined)
          return this._m_extendedParamsPresent;
        this._m_extendedParamsPresent = (this.extensions & 1);
        return this._m_extendedParamsPresent;
      }
    });

    /**
     * The block type determines what block is embedded in the message.
     * For some message types, block type is not relevant and the block type
     * is set to "invalid" or "not_a_block"
     */
    Object.defineProperty(MessageHeader.prototype, 'blockType', {
      get: function() {
        if (this._m_blockType !== undefined)
          return this._m_blockType;
        this._m_blockType = ((this.extensions & 3840) >>> 8);
        return this._m_blockType;
      }
    });

    /**
     * Since protocol v17. For confirm_ack vote-by-hash, this is the number of hashes
     * in the body. For confirm_req request-by-hash, this is the number of hash+root pairs.
     */
    Object.defineProperty(MessageHeader.prototype, 'itemCountInt', {
      get: function() {
        if (this._m_itemCountInt !== undefined)
          return this._m_itemCountInt;
        this._m_itemCountInt = ((this.extensions & 61440) >>> 12);
        return this._m_itemCountInt;
      }
    });

    /**
     * If set, this is a node_id_handshake query. This maybe be set at the
     * same time as the response_flag.
     */
    Object.defineProperty(MessageHeader.prototype, 'queryFlag', {
      get: function() {
        if (this._m_queryFlag !== undefined)
          return this._m_queryFlag;
        this._m_queryFlag = (this.extensions & 1);
        return this._m_queryFlag;
      }
    });

    /**
     * Since protocol version 18 (release 21.3).
     * May be set for "frontier_req" messages.
     * If set, the frontier_req response contains confirmed frontiers for each account.
     */
    Object.defineProperty(MessageHeader.prototype, 'confirmedPresent', {
      get: function() {
        if (this._m_confirmedPresent !== undefined)
          return this._m_confirmedPresent;
        this._m_confirmedPresent = (this.extensions & 2);
        return this._m_confirmedPresent;
      }
    });

    /**
     * Since protocol version 19 (release 24)
     * Must be set for "ascending pull messages" messages. Indicates size of payload of ascending pull message.
     */
    Object.defineProperty(MessageHeader.prototype, 'ascPullSize', {
      get: function() {
        if (this._m_ascPullSize !== undefined)
          return this._m_ascPullSize;
        this._m_ascPullSize = (this.extensions & 65535);
        return this._m_ascPullSize;
      }
    });

    /**
     * Protocol identifier. Always 'R'.
     */

    /**
     * Network ID 'A', 'B' or 'C' for test, beta or live network respectively.
     */

    /**
     * Maximum version supported by the sending node
     */

    /**
     * Version used by the sending node
     */

    /**
     * Minimum version supported by the sending node
     */

    /**
     * Message type
     */

    /**
     * Extensions bitfield
     */

    return MessageHeader;
  })();

  /**
   * Response of the msg_frontier_req TCP request. An all-zero account and frontier_hash signifies the end of the result.
   */

  var FrontierResponse = Nano.FrontierResponse = (function() {
    function FrontierResponse(_io, _parent, _root) {
      this._io = _io;
      this._parent = _parent;
      this._root = _root || this;

      this._read();
    }
    FrontierResponse.prototype._read = function() {
      this.entry = []
      var i = 0;
      do {
        var _ = new FrontierEntry(this._io, this, this._root);
        this.entry.push(_);
        i++;
      } while (!( ((this._io.isEof()) || ((KaitaiStream.byteArrayCompare(this.entry[i].frontierHash, this._root.constBlockZero) == 0))) ));
    }

    var FrontierEntry = FrontierResponse.FrontierEntry = (function() {
      function FrontierEntry(_io, _parent, _root) {
        this._io = _io;
        this._parent = _parent;
        this._root = _root || this;

        this._read();
      }
      FrontierEntry.prototype._read = function() {
        this.account = this._io.readBytes(32);
        this.frontierHash = this._io.readBytes(32);
      }

      /**
       * Public key of account.
       */

      /**
       * Hash of the head block of the account chain.
       */

      return FrontierEntry;
    })();

    return FrontierResponse;
  })();

  /**
   * ascending pull request
   */

  var MsgAscPullReq = Nano.MsgAscPullReq = (function() {
    function MsgAscPullReq(_io, _parent, _root) {
      this._io = _io;
      this._parent = _parent;
      this._root = _root || this;

      this._read();
    }
    MsgAscPullReq.prototype._read = function() {
      this.base = new AscPullBase(this._io, this, this._root);
      this._raw_payload = this._io.readBytes(this._root.header.ascPullSize);
      var _io__raw_payload = new KaitaiStream(this._raw_payload);
      this.payload = new AscPullReqPayload(_io__raw_payload, this, this._root);
    }

    /**
     * ascend pull request acount info payload
     */

    var AccountInfoReqPayload = MsgAscPullReq.AccountInfoReqPayload = (function() {
      function AccountInfoReqPayload(_io, _parent, _root) {
        this._io = _io;
        this._parent = _parent;
        this._root = _root || this;

        this._read();
      }
      AccountInfoReqPayload.prototype._read = function() {
        this.start = this._io.readBytes(32);
        this.startType = this._io.readU1();
      }

      /**
       * block hash or account pubkey to pull from
       */

      return AccountInfoReqPayload;
    })();

    /**
     * ascend pull request block payload
     */

    var BlocksReqPayload = MsgAscPullReq.BlocksReqPayload = (function() {
      function BlocksReqPayload(_io, _parent, _root) {
        this._io = _io;
        this._parent = _parent;
        this._root = _root || this;

        this._read();
      }
      BlocksReqPayload.prototype._read = function() {
        this.start = this._io.readBytes(32);
        this.count = this._io.readU1();
        this.startType = this._io.readU1();
      }

      /**
       * block hash or account pubkey to pull from
       */

      /**
       * max number of blocks to pull
       */

      return BlocksReqPayload;
    })();

    /**
     * payload of ascend pull requests
     */

    var AscPullReqPayload = MsgAscPullReq.AscPullReqPayload = (function() {
      function AscPullReqPayload(_io, _parent, _root) {
        this._io = _io;
        this._parent = _parent;
        this._root = _root || this;

        this._read();
      }
      AscPullReqPayload.prototype._read = function() {
        if (this._parent.base.type == Nano.EnumAscPullType.ACCOUNT_PULL_TYPE) {
          this.accountInfo = new AccountInfoReqPayload(this._io, this, this._root);
        }
        if (this._parent.base.type == Nano.EnumAscPullType.BLOCK_PULL_TYPE) {
          this.blocks = new BlocksReqPayload(this._io, this, this._root);
        }
      }

      return AscPullReqPayload;
    })();

    return MsgAscPullReq;
  })();

  /**
   * Signed telemetry response
   */

  var MsgTelemetryAck = Nano.MsgTelemetryAck = (function() {
    function MsgTelemetryAck(_io, _parent, _root) {
      this._io = _io;
      this._parent = _parent;
      this._root = _root || this;

      this._read();
    }
    MsgTelemetryAck.prototype._read = function() {
      this.signature = this._io.readBytes(64);
      this.nodeid = this._io.readBytes(32);
      this.blockcount = this._io.readU8be();
      this.cementedcount = this._io.readU8be();
      this.uncheckedcount = this._io.readU8be();
      this.accountcount = this._io.readU8be();
      this.bandwidthcap = this._io.readU8be();
      this.peercount = this._io.readU4be();
      this.protocolversion = this._io.readU1();
      this.uptime = this._io.readU8be();
      this.genesisblock = this._io.readBytes(32);
      this.majorversion = this._io.readU1();
      this.minorversion = this._io.readU1();
      this.patchversion = this._io.readU1();
      this.prereleaseversion = this._io.readU1();
      this.maker = this._io.readU1();
      this.timestamp = this._io.readU8be();
      this.activedifficulty = this._io.readU8be();
      if (this._io.pos < this._root.header.telemetrySize) {
        this.unknownData = []
        var i = 0;
        do {
          var _ = this._io.readU8le();
          this.unknownData.push(_);
          i++;
        } while (!(this._io.pos == this._root.header.telemetrySize));
      }
    }

    /**
     * Signature (Big endian)
     */

    /**
     * Public node id (Big endian)
     */

    /**
     * Block count
     */

    /**
     * Cemented block count
     */

    /**
     * Unchecked block count
     */

    /**
     * Account count
     */

    /**
     * Bandwidth limit, 0 indiciates unlimited
     */

    /**
     * Peer count
     */

    /**
     * Protocol version
     */

    /**
     * Length of time a peer has been running for (in seconds)
     */

    /**
     * Genesis block hash (Big endian)
     */

    /**
     * Major version
     */

    /**
     * Minor version
     */

    /**
     * Patch version
     */

    /**
     * Pre-release version
     */

    /**
     * Maker version. 0 indicates it is from the Nano Foundation, there is no standardised list yet for any others.
     */

    /**
     * Number of milliseconds since the UTC epoch
     */

    /**
     * The current network active difficulty.
     */

    return MsgTelemetryAck;
  })();
  Object.defineProperty(Nano.prototype, 'constBlockZero', {
    get: function() {
      if (this._m_constBlockZero !== undefined)
        return this._m_constBlockZero;
      this._m_constBlockZero = this._io.readBytes(32);
      return this._m_constBlockZero;
    }
  });

  /**
   * Message header with message type, version information and message-specific extension bits.
   */

  /**
   * Message body whose content depends on block type in the header.
   */

  return Nano;
})();
return Nano;
}));
