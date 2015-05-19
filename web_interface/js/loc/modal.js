/*
 * Some function that provides essential operations on the modal window
 * thanks to Martin Brugnara
 */

/*
 * Opens a modal 
 * id: modal id as string
 */
function modal_open(id) {
  var modal = document.getElementById(id);
  modal.style.display = "block";
}

/*
 * Closes a modal
 * id: modal id as string
 */
function modal_close(id) {
  var modal = document.getElementById(id);
  modal.style.display = "none";
}

/* automates close on X click */
(function() {
  var cl = document.getElementsByClassName("close");
  for (var i=0; i<cl.length; i++) {
    cl[i].onclick = (function(a) {
      return function() {
        var modal = a.parentNode.parentNode.parentNode.id;
        modal_close(modal);
      };
    }) (cl[i]);
  }
})();