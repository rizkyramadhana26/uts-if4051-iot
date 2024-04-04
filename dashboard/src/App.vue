<template>
  <div class="px-4 flex flex-col h-[100vh]">
    <div class="flex w-full h-[50px] bg-white mt-4 rounded-lg items-center justify-center">
      <div>UTS IOT 13520151 - Sistem Pembayaran Cashless</div>
    </div>
    <div class="w-full rounded-lg grow bg-white my-4 px-4">
      <div class="mt-10 mb-4 ml-2 ">
        <b>SALDO TOTAL: </b> Rp{{ saldo }}
      </div>
      <div class="w-[200px] mb-2">
        <v-form @submit.prevent="submit">
          <v-number-input
            :min="0"
            :model-value="0"
            :step="10000"
            id="topup_nominal"
          ></v-number-input>
          <v-btn class="text-xs" type="submit" block>Top Up</v-btn>
        </v-form>
      </div>
      <v-data-table theme="light" :items="transactions" :headers="headers">
      </v-data-table>
    </div>
  </div>
</template>

<script setup>
import {computed,ref} from 'vue'
import { VNumberInput } from 'vuetify/labs/VNumberInput'
import mqtt from "precompiled-mqtt";

function submit(event){
  client.publish("ESP/INPUT",event.target[0].value)
}

const client = mqtt.connect("ws://192.168.100.45:8883");

client.on("connect", () => {
  client.subscribe("ESP/OUTPUT", (err) => {
    if (!err) {
      console.log("SUCCESS SUBSCRIBING!")
    }
  });
});

client.on("message", (topic, message) => {
  if(topic=='ESP/OUTPUT'){
    const arr = message.toString('utf-8').split(";");
    transactions.value = []
    var idx = 1
    arr.forEach((item) => {
      if(item!=""){
        transactions.value.push({
        no : idx++,
        jenis : item.split(" ")[0],
        nominal : item.split(" ")[1]
      })
      }
    })
  }
});

var transactions = ref([
  {
    no: 1,
    jenis : '+',
    nominal : 251000
  },
  {
    no: 2,
    jenis : '-',
    nominal : 20000
  },
  {
    no: 3,
    jenis : '+',
    nominal : 10000
  }
])

const headers = [
  {
    title : 'No.',
    key : 'no',
    sortable : false
  },
  {
    title : 'Jenis Transaksi',
    key : 'jenis'
  },
  {
    title : 'Nominal',
    key : 'nominal'
  }
]

var saldo = computed(()=>{
  var sum = 0
  transactions.value.forEach(tx => {
    if(tx.jenis=='-'){
      sum -= parseInt(tx.nominal)
    }else{
      sum += parseInt(tx.nominal)
    }
  })
  return sum
})


</script>

